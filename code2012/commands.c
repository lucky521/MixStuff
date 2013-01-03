#include "config.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "TEMU_lib.h"
#include "tracecap.h"
#include "conf.h"
#include "procmod.h"
#include "libfstools.h"
#include "slirp.h"
#include "read_linux.h"
#include "reg_ids.h"
#include "shared/procmod.h"
#include "conditions.h"
#include "readwrite.h"
#include "network.h"
#include "errdet.h"

/* plugin loading */
//#include <dlfcn.h>
//#include <assert.h>
#include "hook_helpers.h"
#include "hookapi.h"
#include "function_map.h"
#include "hook_plugin_loader.h"


static plugin_interface_t tracing_interface; //插件接口

char current_mod[32] = "";
char current_proc[32] = "";

static int taint_sendkey_id = 0;
int keystroke_offset = 0; // Global counter for keystrokes, used for offset
uint32_t current_tid = 0; // Current thread id
static char tracefile[256];


/* Loop induction variable variables */
#define MAX_LOOP_IVS 5
size_t num_loop_ivs = 0;
uint32_t loop_ivs_sarray[MAX_LOOP_IVS];

/* define taint PEB ID *********************/
//PEB指的是进程环境块，是系统内核中的一个结构
#define TAINT_PEB_ID 6666
uint32_t shouldTaintPEB = 0;
//******************************************

static term_cmd_t tracing_info_cmds[] = {
  {NULL, NULL},
};


typedef struct {
  FS_INFO *fs;
  IMG_INFO *img;
  void *bs;
} disk_info_t;

static disk_info_t disk_info[5];

typedef struct {
  uint64_t cluster;
  taint_record_t rec;
  int count;
  FS_INFO *fs;
} taintcluster_t;


//***********************************************************************************
//memtr结构链表的数据结构  	TODO:其意义和作用？ 很抽象，只有地址，大小
typedef struct _memtr{
  uint32_t s_addr; //地址（虚拟地址）
  uint32_t s_size; //大小
  int s_id;	   //ID
  struct _memtr *next;
  struct _memtr *parent;  //一个双向链表
} TAINTMEMETRY;

TAINTMEMETRY *psTMlist = NULL; //memtr结构链表的表头
uint32_t TMCount = 0;          //链表中的结点数目

//************************************************************************************
//往memtr结构链表中加入一个新创建的结点（给出 地址，大小，ID）
void f_taint_mem(uint32_t uAddr, uint32_t uSize, int id)
{
  TAINTMEMETRY *entry = (TAINTMEMETRY*)malloc(sizeof(TAINTMEMETRY));//申请一个结构
  TAINTMEMETRY *tmp = psTMlist;
  /*if(tracepid == 0 || tracecr3 == 0){
    term_printf("not trace any process!\n");
    return;
  }*//*do not need to wait a process start*/
  if(entry == NULL) //如果空间申请失败
  {
    term_printf("out of memory at do_taint_memory!\n");
    return;
  }
  //出初始化这个结构内容
  entry->next = entry->parent = NULL; 
  entry->s_addr = uAddr; 
  entry->s_size = uSize;
  entry->s_id = id;

  //把新的entry插入memtr结构链表中（采用头插法）
  if(psTMlist == NULL) {
    psTMlist = entry; 
  }else{
    entry->next = tmp;
    tmp->parent = entry;
    psTMlist = entry;
 }
 TMCount ++;
 term_printf("add new entry for 0x%0x !\n", uAddr);
 return;
}

/*
addr is a string, like: 0x400012
size is a string too, like: 0xf2
*/
//把地址，大小，ID以标准的形式创建到memtr结构链表中
void do_taint_memory(const char* addr, const char* size, int id)
{
  uint32_t uAddr, uSize;
  
  sscanf(addr, "%x", &uAddr); //把字符串以十六进制的形式存成uaddr
  sscanf(size, "%x", &uSize); //同上
  f_taint_mem(uAddr, uSize, id); //往memtr结构链表中加入新创建的结点
}

//********************************************************************************
/*
 do_taint_peb.
 this function taint PEB struct's memory.这个函数用来打PEB结构的污点。
*/
void do_taint_peb()
{
  shouldTaintPEB=1;
}
void taint_peb(void)
{
  uint32_t pebptr;
  //读取出PED结构的地址
  if(TEMU_memory_rw(TEMU_cpu_segs[R_FS].base+0x30, (uint8_t*)&pebptr, 4, 0) == -1){
    term_printf("read peb pointer on do_taint_peb failed!\n");
	return;
  }
  f_taint_mem(pebptr+2, 0x1, TAINT_PEB_ID);//为peb结构的地址，大小，id创建结点加入到memtr结构链表中
}


//**********************************************************************************
/* do taint rdtsc */

void do_taint_rdtsc(void)
{
  iTracingRdtsc = 1;
}

static void tracing_taint_disk  (uint64_t addr, uint8_t * record, void *opaque) {
        return;
}

//**************************************************************************************
static void taint_disk_block(char filename[], uint64_t block, int size,
  uint32_t origin, int disk_index, uint32_t offset)
{
  int i;
  taint_record_t records[64]; //设一个临时的污点记录集（可放64个污点纪录）
  printf("%Lx:%d[%d] ", block, size, origin);
  bzero(records, sizeof(records)); //置records的内容为0
  
  for (i = 0; i < 64; i++) {
    records[i].numRecords = 1;//已经使用了1个污点字节纪录
    records[i].taintBytes[0].source = TAINT_SOURCE_FILE_IN; //污点来源：文件
    records[i].taintBytes[0].origin = origin;               //网络标记
    records[i].taintBytes[0].offset = offset + i;	    //偏移量
    memset(&(records[i].taintBytes[1]), 0,
      (MAX_NUM_TAINTBYTE_RECORDS-1)*sizeof(TaintByteRecord)); //第二个污点字节的位子清干净
  }
  
  for (i = 0; i < size; i += 64)
    taintcheck_taint_disk((block * disk_info[disk_index].fs->block_size +
                           i + 0x7e00) / 64,
                          (uint64_t) - 1,
                          0, 64, (uint8_t *) records,
                          disk_info[disk_index].bs); //TODO：把纪录放进磁盘块里？
}

//文件名，设备号，污点id
void do_taint_file(char *filename, int dev_index, uint32_t taint_id)
{

  FS_INFO *fs;
  int i;
  uint32_t offset;
  extern uint32_t tsk_errno;

  tsk_errno = 0;

  term_printf("Tainting disk %d file %s\n", dev_index, filename);

  if (!(fs = disk_info[dev_index].fs)) {
    term_printf("Could not find disk_info\n");
    return;
  }

  char *path = strdup(filename);
  if (!path) {
    term_printf("Empty path\n");
    return;
  }
  if (!fs_ifind_path(fs, IFIND_PATH | IFIND_PATH, path) &&
      !fs_icat(fs, 0, found_path_inode, 0, 0, 1 << 5))
  {
    for (i = 0, offset=0; i < found_icat_nblock; i++) {
      if (taint_id == 0)        //it means we are tainting a directory
        taint_disk_block(filename, found_icat_blocks[i].addr,
          found_icat_blocks[i].size,
          //we generate an ID in [400, 1000)
          (found_icat_blocks[i].addr % 600) + 400, dev_index, offset);
      else
        taint_disk_block(filename, found_icat_blocks[i].addr,
                         found_icat_blocks[i].size, taint_id, dev_index, offset);

    offset += found_icat_blocks[i].size;
    }
    term_printf("Tainted file %s\n", filename);
  }
  else {
    term_printf("Could not find file\n");
  }
  printf("\n");
  free(path);

}



void do_taint_sendkey(const char *string, int id)
{
  taint_sendkey_id = id;
  do_send_key(string);   //TODO： 在monitor.c中实现。
}


//判定是否是内核指令
int is_kernel_instruction()
{
    return ((*TEMU_cpu_hflags & HF_CPL_MASK) != 3); 
}


//*****************************************************************************************
//确定出污点的传播方式  
static void tracing_taint_propagate(int nr_src,   //源操作数的个数
                            taint_operand_t * src_oprnds,
                            taint_operand_t * dst_oprnd,
                            int mode)
{
  if (0==tracing_table_lookup && 2==nr_src && PROP_MODE_MOVE==mode) {
    /* if first is untainted, clear taint info of second arg */
    if (src_oprnds[0].taint==0) {
    /* clear taint info of destination  把目的操作数的污点信息清空*/ 
    if (0==dst_oprnd->type) /* register  */
      taintcheck_taint_register(
        dst_oprnd->addr>>2, dst_oprnd->addr&3,
        dst_oprnd->size,
        0, NULL); // 看taint=0, records=null
    else /* memory */
      taintcheck_taint_memory(
        dst_oprnd->addr,
        dst_oprnd->size,
        0, NULL); // 看taint=0, records=null
    eh.tp = TP_MEMREAD_INDEX; //传播方式设置为：读污点内存
    return;
    } else
    nr_src = 1;
  }

  /* Propagate taint, this needs to be done for all instructions */
  default_taint_propagate(nr_src, src_oprnds, dst_oprnd, mode);//在taint_check.c中。

  /* No need to set tp in entry header if not tracing */
  if ((!tracing_start_condition) || (tracepid == 0))
    return;

  /* No need to set tp in entry header if not 
   * tracing kernel and kernel instruction */
  if ( is_kernel_instruction() && !tracing_kernel() )
    return;

  /* Instruction propagated taint. Set tp in entry header */
  if (eh.tp == TP_NONE) eh.tp = TP_SRC; //传播方式为：从源地传向目的地

  if(mode == PROP_MODE_MOVE) {
     /* Check if it is a memory write with tainted index */
    if ((dst_oprnd->type == 1)) { //目的操作数是寄存器类型
      uint64_t a0_tainted;
      a0_tainted = taintcheck_register_check(R_A0, 0, 4, NULL);
      if (a0_tainted != 0)
        eh.tp = TP_MEMWRITE_INDEX; //传播方式为：污点内存写
    }

    if(nr_src > 1) {
      if (src_oprnds[0].taint == 0) {
        eh.tp = TP_MEMREAD_INDEX; //传播方式为：污点内存读
      }
      else {
        eh.tp = TP_SRC;
      }
    }
  }

}


static void tracing_guest_message(char *message)
{
  handle_message(message);
  switch (message[0]) {
  case 'P':
    parse_process(message);
    break;
  case 'M':
    parse_module(message);
    break;
  }
}

static int tracing_block_begin()
{
  hookapi_check_call(temu_plugin->monitored_cr3 == TEMU_cpu_cr[3] && 
                    !TEMU_is_in_kernel());

  /* If not tracing kernel and kernel instruction , return */
    if ( is_kernel_instruction() && !tracing_kernel() )
      return 0;

  /* If not tracing by pid or by name, return */
  if  ((tracepid == 0) && (!procname_is_set()))
    return 0;

  /* Get thread id */
  current_tid = get_current_tid();

  tmodinfo_t *mi;
  mi = locate_module(*TEMU_cpu_eip, TEMU_cpu_cr[3],
                     current_proc);
  strncpy(current_mod, mi ? mi->name : "unknown",31);
  current_mod[31] = '\0';

  if (procname_is_set()) {
    char temp[64];
    uint32_t pid;


    find_process(TEMU_cpu_cr[3], temp, &pid);
    if (procname_match(temp)) {
      do_tracing(pid, tracefile);
      term_printf("Tracing %s\n", procname_get());
      procname_clear();
      if(shouldTaintPEB != 0){
        shouldTaintPEB = 0;
        taint_peb();
      }
    }
  }

  if (modname_is_set()) {
      if (modname_match(current_mod) &&
	  (temu_plugin->monitored_cr3 == TEMU_cpu_cr[3]))
      {
	  tracing_start_condition = 1;
	  modname_clear();
      }
  }

  return 0;
}

//记录下键盘按键污点
static void tracing_send_keystroke(int reg)
{
  taint_record_t record;

  /* If not tracing, return */
  if  (tracepid == 0)
    return;

  //term_printf ("Keystroke received: %d\n",taint_sendkey_id);

  if (taint_sendkey_id) {
    uint32_t keystroke = TEMU_cpu_regs[reg]; 
    term_printf ("Tainting keystroke: %d %08X\n", reg,keystroke);
    //这个在操作出能在终端看到。 输出第reg号寄存器中的值。
    
    record.numRecords = 1;//使用了一个污点字节
    record.taintBytes[0].source = TAINT_SOURCE_KEYBOARD_IN; //污点来源：键盘按键
    record.taintBytes[0].origin = taint_sendkey_id; //网络标记
    record.taintBytes[0].offset = keystroke_offset; // 按键数偏移
    memset(&(record.taintBytes[1]), 0,
      (MAX_NUM_TAINTBYTE_RECORDS-1)*sizeof(TaintByteRecord)); //先把位置打扫干净

    taintcheck_taint_register(reg, 0, 1, 1, (uint8_t *) &record);//打入污点
    taint_sendkey_id = 0;
    keystroke_offset++; //每记录下一个键盘按键后，偏移要自增
  }
}


static void tracing_bdrv_open(int index, void *opaque)
{
  if ((disk_info[index].img =
       img_open("qemu", 1, (const char **) &opaque)) == NULL) {
    tsk_error_print(stderr);
    return;
  }
  if (!(disk_info[index].fs = fs_open(disk_info[index].img, 0x7e00, NULL))
      && !(disk_info[index].fs =
           fs_open(disk_info[index].img, 0x00, NULL))) {
    tsk_error_print(stderr);
    if (tsk_errno == TSK_ERR_FS_UNSUPTYPE)
      fs_print_types(stderr);
    disk_info[index].img->close(disk_info[index].img);
    return;
  }
  disk_info[index].bs = opaque;
}


static void stoptracing()
{
  term_printf("Received Signal: STOP\n");
  tracing_stop();
}

static void killtemu()
{
  term_printf("Received Signal: KILL\n");
  exit(EXIT_KILL_SIGNAL);
}

//*******************************************************************************
//载入钩子
void do_load_hooks (const char *hooks_dirname, const char *plugins_filename)
{
  if (strcmp(plugins_filename, "") != 0)
    strncpy(hook_plugins_filename, plugins_filename, 256);
  if (strcmp(hooks_dirname, "") != 0)
    strncpy(hook_dirname, hooks_dirname, 256);

  // Load hooks if requested via TEMU monitor
  load_hook_plugins(&(temu_plugin->monitored_cr3),
    hook_plugins_filename,
    hook_dirname,
    &g_plugin_info,
    ini);
}



int uint32_compare(const void* u1, const void* u2) {
  return *((uint32_t *) u1) - *((uint32_t *) u2);
}



void do_add_iv_eip(uint32_t eip)
{
  if (num_loop_ivs >= MAX_LOOP_IVS) {
    term_printf("max no. eips allowed (%d) is reached.\n", MAX_LOOP_IVS);
  }
  else {
    loop_ivs_sarray[num_loop_ivs++] = eip;
    qsort(&(loop_ivs_sarray[0]), num_loop_ivs, sizeof(uint32_t),
      uint32_compare);
  }
}

//******************************************************************************************
//跟踪初始化
static int tracing_init()
{
  function_map_init();
  init_hookapi();
  procmod_init();

  // setup signal handler to stop tracing
  signal(SIGUSR1, stoptracing);

  // SIGUSR2 is used by QEMU

  // setup signal handler to exit TEMU
  signal(SIGTERM, killtemu);

  procname_clear(); 

  // this is needed for file tainting
  qemu_pread = (qemu_pread_t)TEMU_bdrv_pread;

  // Parse configuration file
  check_ini(ini_main_default_filename);

  return 0;
}

static void tracing_cleanup()
{
  //TODO: other cleanup stuff, like function hooks, log files
    unload_hook_plugins();
    procmod_cleanup();
    hookapi_cleanup();
    function_map_cleanup();
}

void do_tracing_stop()
{
  tracing_stop();
}

//******************************************************************************************
// trace 这个命令在实际操作中最关键
void do_tracing(uint32_t pid, const char *filename)
{
  /* if pid = 0, stop trace */
  if (0 == pid)
    tracing_stop();
  else {
    int retval = tracing_start(pid, filename); //关键在这里。
    if (retval < 0)
      term_printf("Unable to open log file '%s'\n", filename);
  }

  /* Print configuration variables */
  //print_conf_vars(); 
}

void do_tracing_by_name(const char *progname, const char *filename)
{
  /* If process already running, start tracing */
  uint32_t pid = find_pid_by_name(progname);
  uint32_t minus_one = (uint32_t)(-1);
  if (pid != minus_one) {
    do_tracing(pid,filename);
    return;
  }

  /* Otherwise, start monitoring for process start */
  procname_set(progname); 
  strncpy(tracefile, filename, 256);
  term_printf ("Waiting for process %s to start\n", progname);

#if 0
  /* Print configuration variables */
  print_conf_vars(); 
#endif
}
//******************************************************************************************

void do_save_state(uint32_t pid, uint32_t address, const char *filename)
{
  int retval;
  retval = save_state_init(pid, address, filename);
  if (retval == -1)
    term_printf("Unable to open log file '%s'\n", filename);
}

void do_guest_modules(uint32_t pid)
{
  list_guest_modules(pid);
}


void do_clean_iv_eips()
{
  num_loop_ivs = (size_t) 0;
}

void taint_loop_ivs()
{
    uint64_t mask = 0;
    taint_record_t taintrec[MAX_OPERAND_LEN]; /* taint_rec[] to write */
    int regnum = -1;
    int offset = 0;
    int length = 0;
    int index_itr = 0;
    int i=0;

    if (!bsearch(&(eh.address), &(loop_ivs_sarray[0]), num_loop_ivs,
     sizeof(uint32_t), uint32_compare))
  return;      /* skip if the current eip is not in loop_ivs_sarray[] */

    i = 0;                               /* only care about dest operand */
    if (eh.operand[i].type != TRegister)  /* ignore if it's not register */
  return;                 /* replace with continue; if it's a loop */

    /* get original taint recs */
    //regnum = regmapping[eh.operand[i].addr - 100];
    regnum = get_regnum(eh.operand[i]);
    offset = getOperandOffset(&eh.operand[i]);
    length = eh.operand[i].length;
    taintcheck_register_check(regnum, offset, length,
            (uint8_t *) taintrec);

    term_printf("logic reached\n");
    for (index_itr =0; index_itr < length; ++index_itr) {
  /* we're overwriting any existing taint records in the register */
  /* except loop_iv record from the same eip origin */
  /* in such case, we increment the counter (e.g. offset field) */
  if (taintrec[index_itr].taintBytes[0].source==TAINT_SOURCE_LOOP_IV
      && taintrec[index_itr].taintBytes[0].origin == eh.address) {
      ++(taintrec[index_itr].taintBytes[0].offset);
  } else {
      taintrec[index_itr].taintBytes[0].source =
    TAINT_SOURCE_LOOP_IV;
      taintrec[index_itr].taintBytes[0].origin = eh.address;
      taintrec[index_itr].taintBytes[0].offset = 1;
  }
  taintrec[index_itr].numRecords = 1;
  term_printf("IV tainted is %5s, EIP = 0x%8x, count = %5d\n",
        reg_name_from_id(eh.operand[i].addr),
        eh.address, taintrec[index_itr].taintBytes[0].offset);
    }

    mask = (1ULL<<eh.operand[i].length)-1;
    taintcheck_taint_register(regnum, offset, length, mask,
          (uint8_t *) taintrec);
}

//******************************************************************************************
//开始跟踪指令
void tracing_insn_begin()
{
  TAINTMEMETRY *tmp = psTMlist; 
  TAINTMEMETRY *ptmp = (tmp!=NULL)?tmp->parent:NULL;
  hook_taint_record_t record; //这个record是临时数据结构，相当于一个模子
  uint32_t paddr = 0;
  uint64_t taint = 0; 

  //判断是否能跟踪
  /* If tracing start condition not satisified, or not tracing return */
  if ((!tracing_start_condition) || (tracepid == 0))
    return ;

  /* If not tracing kernel and kernel instruction , return */
  if ( is_kernel_instruction() && !tracing_kernel() )
    return;


    
  /*try to taint specific virtual memory*/
  if(tracecr3 != 0 && tracecr3 == TEMU_cpu_cr[3]){
    /*start tainting virtual memory in TAINTMEMETRY list */
    //跟踪memtr链表中的每个结点的虚拟地址，去打污点
    while(tmp != NULL){
      paddr = TEMU_get_phys_addr(tmp->s_addr); //虚拟地址->物理地址
      if(paddr == -1){
        tmp = tmp->next;
        continue;
      }

      //先利用临时的record模子，设定一下污点信息。
      memset(&record, 0, sizeof(record));//把record清空，然后填入信息。
      
      record.source = TAINT_SOURCE_KEYBOARD_IN; 
      record.origin = tmp->s_id;	//污点字节的网络标记：结点的id
      record.offset = keystroke_offset; 
      
      /************taint 打污点 *****************************/
      taint_mem(tmp->s_addr, tmp->s_size, (uint8_t*)&record);
      
      //delete entry from list 处理掉这个结点
      if(tmp->parent == NULL){
        psTMlist = tmp->next;
      }else{
        tmp->parent->next = tmp->next;
      }
      ptmp = tmp;
      tmp = tmp->next;
      free(ptmp);
      TMCount --;
    }//end of while...
  }// end of if...

  /* Clear flags before processing instruction */

  // Flag to be set if the instruction is written
  insn_already_written = 0;

  // Flag to be set if instruction encounters a page fault
  // NOTE: currently not being used. Tracing uses it to avoid logging twice
  // these instructions, but was missing some
  has_page_fault = 0;

  // Flag to be set if instruction accesses user memory
  access_user_mem = 0;

  /* Check if this is a system call */
  if (conf_log_external_calls) {
    uint32_t eip = *TEMU_cpu_eip; 
    struct names_t *names = query_name(eip); //根据eip查到函数名，模块名（function_map.cpp）

	//XXX:此处如果eip是特定api函数的入口地址，说明刚执行完了了call～指令，
	//    eip中刚刚被放入了api入口地址，下一条要执行的指令就要是被调函数了。
	//    此时栈中。栈顶是函数的返回地址，就是执行完api函数后要返回的地址。
	//    再往下是函数的参数了。

    //撰写calls文件。凡是调用了的api都会记录下来。
    if ((names != NULL) && (calllog)) {
      if ((names->fun_name != NULL) && (names->mod_name != NULL)) {
	fprintf(calllog,"Process %d call to %s::%s @ EIP: 0x%08x\n",
	  tracepid,names->mod_name,names->fun_name,eip);
      }
      else {
	fprintf(calllog,"Process %d call to ?::? @ EIP: 0x%08x\n", 
	  tracepid,eip);
      }
    }
	//TODO：在此添加。追击特定的入口地址。
    
  }



  CanTraceRdtsc = 0;

  /* Disassemble the instruction */
  decode_address(*TEMU_cpu_eip, &eh, skip_taint_info); //根据eip中的这条指令，制作其entryheader

#ifdef TAINT_LOOP_IVS
  /* If not tracing, skip */
  if (tracepid != 0)
      taint_loop_ivs();
#endif

#ifdef INSN_INFO
  savedeip = *TEMU_cpu_eip;
#endif

}


//本函数涉及到了trace文件的写入过程
void tracing_insn_end()
{
  uint32_t base_id=0, reg_offset=0, size=0;
  taint_record_t record;
  
  /* If tracing start condition not satisified, or not tracing return */
  if ((!tracing_start_condition) || (tracepid == 0))
    return ;

  /* If not tracing kernel and kernel instruction , return */
  if ( is_kernel_instruction() && !tracing_kernel())
    return;

  /* If partially tracing kernel but did not access user memory, return */
  if (is_kernel_instruction()) {
      if (tracing_kernel_partial() && (!access_user_mem))
	  return;
      if (tracing_kernel_tainted() && (!insn_tainted))
	  return;
  }

   /* If instruction already written, return */
  if (insn_already_written == 1)
    return;

  /* Update the eflags */
  eh.eflags = *TEMU_cpu_eflags;
  eh.df = *TEMU_cpu_df;

  /* Update the thread id */
  eh.tid = current_tid;

  /* Clear eh.tp if inside a function hook */
  if (skip_taint_info > 0) eh.tp = TP_NONE;
  else {
    /* Update eh.tp if rep instruction */
    if ((eh.operand[2].usage == counter) && (eh.operand[2].tainted != 0))
      eh.tp = TP_REP_COUNTER;

    /* Updated eh.tp if sysenter */
    else if ((eh.rawbytes[0] == 0x0f) && (eh.rawbytes[1] == 0x34))
      eh.tp = TP_SYSENTER;
  }

  /* Split written operands if requested */
  if (conf_write_ops_at_insn_end) {
    update_written_operands (&eh);
  }

  /* Write the disassembled instruction to the trace */
  //根据eh结构，去写入文件中。 注意这有与main.ini配置相关的东西，并且配置有优先级。
  //1,是不是只纪录污点指令； 2，是不是第一个污点出现后才记录； 
  if (tracing_tainted_only()) {
    if (insn_tainted)
      write_trace(tracelog,&eh); 
  }
  else {
    if (conf_trace_only_after_first_taint) {
      if ((received_tainted_data == 1) && (has_page_fault == 0)) {
	write_trace(tracelog,&eh);
      }
    }
    else {
      if (has_page_fault == 0) write_trace(tracelog,&eh);
    }
  }

  /* taint rdtsc's return value: edx and eax */
  if(CanTraceRdtsc){
    //taint edx and eax
    term_printf("tainting rdtsc at: 0x%0x\n", eh.address);
    memset(&record, 0, sizeof(taint_record_t));

    record.numRecords=1;
    record.taintBytes[0].source = TAINT_SOURCE_RDTSC;
    record.taintBytes[0].origin = 2011;
    record.taintBytes[0].offset = RdtscOffset;
        
    reg_index_from_id(eax_reg, &base_id, &reg_offset, &size);
    taintcheck_taint_register(base_id, reg_offset, size, (1<<size)-1, (uint8_t*)&record);
    //reg_index_from_id(edx_reg, &base_id, &reg_offset, &size);
    //taintcheck_taint_register(base_id, reg_offset, size, (1<<size)-1, (uint8_t*)&record);
    RdtscOffset++;
  }
}


int tracing_cjmp(uint32_t t0)
{
  /* No need to set tp in entry header if not tracing */
  if ((!tracing_start_condition) || (tracepid == 0))
    return 0;

  /* No need to set tp in entry header if not 
   * tracing kernel and kernel instruction */
  if ( is_kernel_instruction() && !tracing_kernel() )
    return 0;

  /* Set entry header flag for tainted cjmp */
  eh.tp = TP_CJMP;

  return 0;
}


void set_table_lookup(int state)
{
  if (state) {
    tracing_table_lookup = 1;
    term_printf("Table lookup on.\n");
  }
  else {
    tracing_table_lookup = 0;
    term_printf("Table lookup off.\n");
  }
}


/* Param format
<pid>:<traceFilename>:<detectMask>::<pidToSignal>:<processName>
*/
void tracing_after_loadvm(const char*param)
{
  char buf[256];
  strncpy(buf, param, sizeof(buf) - 1);
  buf[255] = '\0';
  int pid_to_signal = 0;

  char *pid_str = strtok(buf, ":");
  if (!pid_str)
    return;

  char *trace_filename = strtok(0, ":");
  if (!trace_filename)
    return;

  char *detect_mask_str = strtok(0, ":");
  if (!detect_mask_str)
    return;

  char *pid_to_signal_str = strtok(0, ":");

  char *process_name = strtok(0, ":");

  char *end = pid_str;
  int pid = (int) strtol (pid_str, &end, 10);
  if (end == pid_str) {
    pid = -1;
  }

  /* If no PID or Process_name, return */
  if ((process_name == NULL) && (pid == -1)) {
    term_printf("PARAM: %s\n", param);
    term_printf("START: %p END: %p\n", pid_str, end);
    term_printf("No PID or Process_name provided\n");
    return;
  }

  end = detect_mask_str;
  unsigned int detect_mask =
    (unsigned int) strtol (detect_mask_str, &end, 16);
  if (end == detect_mask_str) {
    term_printf("PARAM: %s\n", param);
    term_printf("START: %p END: %p\n", detect_mask_str, end);
    term_printf ("No detect mask provided\n");
    return;
  }

  if (pid_to_signal_str) {
    end = pid_to_signal_str;
    pid_to_signal = (int) strtol (pid_to_signal_str, &end, 10);
    if (end == pid_to_signal_str) {
      pid_to_signal = 0;
    }
  }

  term_printf ("PID: %d MASK: 0x%08x PID2SIGNAL: %d PROCESS_NAME: %s\n",
    pid, detect_mask, pid_to_signal, process_name);

  /* Enable emulation */
  do_enable_emulation();

  /* Taint the network 网络来源要打污点*/
  do_taint_nic(1);

  /* Filter traffic (read from ini configuration file) */
  print_nic_filter();

  /* Enable detection */
  enable_detection(detect_mask);

  /* OS dependant initialization */
  if (0 == taskaddr)
    init_kernel_offsets();
  if (0xC0000000 == kernel_mem_start) /* linux */
    update_proc(0);

  /* Load hooks */
  do_load_hooks("","");

  /* Start trace */
  if (process_name == NULL)
    do_tracing(pid, trace_filename);
  else
    do_tracing_by_name(process_name,trace_filename);

  /* Send signal to notify that trace is ready */
  //if (pid_to_signal != 0) kill(pid_to_signal,SIGUSR1);
  int pipe_fd = open("/tmp/temu.pipe",O_WRONLY);
  write(pipe_fd,"OK",2);
  close(pipe_fd);

}

static term_cmd_t tracing_term_cmds[] = {
  /* taint rdtsc insn  ————很特殊的rdtsc指令*/
  {"taint_rdtsc", "", do_taint_rdtsc, "", "taint rdtsc instruction"},
  /* taint PEB memory   ————进程环境块 */
  {"taint_peb", "", do_taint_peb, "", "taint PEB memory"},
  /*new operation for taint memory  ————打污点操作*/
  {"taint_mem", "ssi", do_taint_memory,
   "addr size id", "taint memory in the process"},
  /* operations to set taint source  ————设置污点来源操作 */
  {"taint_sendkey", "si", do_taint_sendkey,
   "key id", "send a tainted key to the guest system"},
  {"taint_nic", "i", do_taint_nic,
   "state", "set the network input to be tainted or not"},
  {"taint_file", "sii", do_taint_file,
   "filename disk_index first_offset", "taint the content of a file on disk"},

  /* operating system information */
  {"guest_ps", "", list_procs,
   "", "list the processes on guest system"},
  {"guest_modules", "i", do_guest_modules,
   "pid", "list the modules of the process with <pid>"},
  {"linux_ps", "", do_linux_ps,
   "", "list the processes on linux guest system"},

  /* operations for attack detection */
  { "detect", "ss", do_detect,
    "type <on|off>", "turn on/off the detection for the following "
    "type attacks: "
    "tainteip, nullptr, exception, processexit, all. all are off "
    "by default." },
  { "action", "s", do_action,
    "type", "launch one of the following actions after attack detection: "
    "none, terminate(default), stopvm"},

  /* operations to record instruction trace */
  { "trace", "iF", do_tracing,
    "pid filename",
    "save the execution trace of a process into the specified file"},
  { "tracebyname", "sF", do_tracing_by_name,
    "name filename",
    "save the execution trace of a process into the specified file"},
  { "trace_stop", "", do_tracing_stop,
    "", "stop tracing current process(es)"},
  { "tc_modname", "s", tc_modname,
    "modulename", "start saving execution trace upon entering the "
    "specified module"},
  { "tc_address", "i", tc_address,
    "codeaddress", "start saving execution trace upon reaching the "
    "specified virtual address"},
  { "tc_address_start", "ii", tc_address_start,
    "codeaddress timehit", "start saving execution trace upon reaching "
    "the specified virtual address for the (timehit+1)th times since "
    "the call of this tc_address_start command"},
  { "tc_address_stop", "ii", tc_address_stop,
    "codeaddress timehit", "stop saving execution trace upon reaching the "
    "specified virtual address for the (timehit+1)th times since the "
    "storing of execution trace"},

  /* set taint or tracing filters */
  { "table_lookup", "i", set_table_lookup,
      "state", "set flag to propagate tainted memory index"},
  { "ignore_dns", "i", set_ignore_dns,
      "state", "set flag to ignore received DNS packets"},
  { "taint_nic_filter", "ss", (void (*)())update_nic_filter,
      "<clear|proto|sport|dport|src|dst> value", 
      "Update filter for tainting NIC"},
  { "filter_tainted_only", "i", set_tainted_only,
    "state", "set flag to trace only tainted instructions"},
  { "filter_kernel_all", "i", set_kernel_all,
    "state", "set flag to trace all kernel instructions in addition to "
    "user instructions"},
  { "filter_kernel_tainted", "i", set_kernel_tainted,
    "state", "set flag to trace tainted kernel instructions in addition to "
    "user instructions"},
  { "filter_kernel_partial", "i", set_kernel_partial,
    "state", "set flag to trace kernel instructions that modify user "
    "space memory"},

  /* operations to record memory state */
  {"save_state", "iis", do_save_state,
   "pid address filename",
   "save the state (register and memory) of a process when its execution "
   "hits the specified address "
   "(address needs to be the first address in a basic block)"},

  /* operations for induction variables */
  { "add_iv_eip", "i", do_add_iv_eip,
    "eip", "add a new eip to a list of know induction variable eips"},
  { "clean_iv_eips", "", do_clean_iv_eips,
    "", "clean up a list of induction variable eips"},

  /* operations for hooks */
  { "load_hooks", "FF", do_load_hooks,
    "hooks_dirname  plugins_filename",
  "change hooks paths (hook directory and plugins.active)"},

  {NULL, NULL},
};

plugin_interface_t * init_plugin()
{
  if (0x80000000 == kernel_mem_start)
    comparestring = strcasecmp;
  else
    comparestring = strcmp;

  tracing_interface.plugin_cleanup = tracing_cleanup;
  tracing_interface.taint_record_size = sizeof(taint_record_t);
  tracing_interface.taint_propagate = tracing_taint_propagate;
  tracing_interface.guest_message = tracing_guest_message;
  tracing_interface.block_begin = tracing_block_begin;
  tracing_interface.insn_begin = tracing_insn_begin;
  tracing_interface.insn_end = tracing_insn_end;
  tracing_interface.term_cmds = tracing_term_cmds;
  tracing_interface.info_cmds = tracing_info_cmds;
  tracing_interface.send_keystroke = tracing_send_keystroke;
  tracing_interface.bdrv_open = tracing_bdrv_open;
  tracing_interface.taint_disk = tracing_taint_disk;
  tracing_interface.nic_recv = tracing_nic_recv;
  tracing_interface.nic_send = tracing_nic_send;
  tracing_interface.eip_tainted = tainteip_detection;
  tracing_interface.after_loadvm = tracing_after_loadvm;//
  tracing_interface.cjmp = tracing_cjmp;
  removeproc_notify = procexit_detection;

  tracing_init ();
  return &tracing_interface;
}
