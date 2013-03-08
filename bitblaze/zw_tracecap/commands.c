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

//*******************************************************************************************
//these variable is defined by liulu
  uint32_t lueax=0,ret_addr=0,ret_eax=0;

  uint32_t virAlloc_ret_addr=0,writepro_ret_addr=0;

  //openprocess
  uint32_t OpenPid=0,openpro_ret_addr=0;
  

  unsigned char buffer[160000];//zhu ru nei rong
  int flag_917 = 0;
  uint32_t inject_addr=0,inject_addr_label=0;
  
  
  uint32_t liu1=0,liu2=0,liu3=0,liu4=0,liu5=0,liu6=0,liu7=0,liu8=0,liu9=0,liu10=0;
  uint32_t ret1=0,ret2=0,ret3=0,ret4=0,ret5=0,ret6=0,ret7=0,ret8=0,ret9=0,ret10=0;
  uint32_t iu1=0,iu2=0,iu3=0,iu4=0,iu5=0,iu6=0,iu7=0,iu8=0,iu9=0,iu10=0;
  uint32_t *buf1,*buf2,*buf3,*buf4,*buf5,*buf6,*buf7,*buf8,*buf9;
  uint32_t cmd1=0,cmd2=0,cmd3=0,cmd4=0;
  uint32_t shellpath[260];
  uint32_t recv_ret_addr=0,recv_buf_addr=0;
  uint32_t recv_buf[200];

  //test-2013-3-5 try to taint getlocaltime()
  uint32_t getlocaltime_ret_addr;
  uint32_t getlocaltime_para;


  //test-2013-3-7 taint gethostbyname()
  uint32_t gethostbyname_ret_addr,gethostbyname_ret_value,addr_of_h_name;

  
  //string cmp
  uint32_t cmpstr1[30],cmpstr2[30]; //32wei
  char realstr1[100],realstr2[100]; //8wei
  int p,q,i_917;

  //test-929
  uint32_t wsncmp_ret_addr=0,cmparestringA_ret_addr=0,rtlequal_ret_addr=0;

  //10.17
  
  
//***********************************************************************************************


static plugin_interface_t tracing_interface;

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
/* define taint PEB ID */
#define TAINT_PEB_ID 6666
uint32_t shouldTaintPEB = 0;

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

typedef struct _memtr{
  uint32_t s_addr;
  uint32_t s_size;
  int s_id;
  struct _memtr *next;
  struct _memtr *parent;
} TAINTMEMETRY;

TAINTMEMETRY *psTMlist = NULL;
uint32_t TMCount = 0;

void f_taint_mem(uint32_t uAddr, uint32_t uSize, int id)
{
  TAINTMEMETRY *entry = (TAINTMEMETRY*)malloc(sizeof(TAINTMEMETRY));
  TAINTMEMETRY *tmp = psTMlist;
  /*if(tracepid == 0 || tracecr3 == 0){
    term_printf("not trace any process!\n");
    return;
  }*//*do not need to wait a process start*/
  if(entry == NULL)
  {
    term_printf("out of memory at do_taint_memory!\n");
    return;
  }

  entry->next = entry->parent = NULL;
  entry->s_addr = uAddr;
  entry->s_size = uSize;
  entry->s_id = id;
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
void do_taint_memory(const char* addr, const char* size, int id)
{
  uint32_t uAddr, uSize;
  
  sscanf(addr, "%x", &uAddr);
  sscanf(size, "%x", &uSize);
  f_taint_mem(uAddr, uSize, id);
}

/*
do_taint_peb.
this function taint PEB struct's memory.
*/
void do_taint_peb()
{
  shouldTaintPEB=1;
}
void taint_peb(void)
{
  uint32_t pebptr;
  if(TEMU_memory_rw(TEMU_cpu_segs[R_FS].base+0x30, (uint8_t*)&pebptr, 4, 0) == -1){
    term_printf("read peb pointer on do_taint_peb failed!\n");
	return;
  }
  f_taint_mem(pebptr+2, 0x1, TAINT_PEB_ID);
}

/* do taint rdtsc */

void do_taint_rdtsc(void)
{
  iTracingRdtsc = 1;
}

static void tracing_taint_disk (uint64_t addr, uint8_t * record, void *opaque) {
        return;
}

static void taint_disk_block(char filename[], uint64_t block, int size,
  uint32_t origin, int disk_index, uint32_t offset)
{
  int i;
  taint_record_t records[64];

  printf("%Lx:%d[%d] ", block, size, origin);

  bzero(records, sizeof(records));
  for (i = 0; i < 64; i++) {
    records[i].numRecords = 1;
    records[i].taintBytes[0].source = TAINT_SOURCE_FILE_IN;
    records[i].taintBytes[0].origin = origin;
    records[i].taintBytes[0].offset = offset + i;
    memset(&(records[i].taintBytes[1]), 0,
      (MAX_NUM_TAINTBYTE_RECORDS-1)*sizeof(TaintByteRecord));
  }
  for (i = 0; i < size; i += 64)
    taintcheck_taint_disk((block * disk_info[disk_index].fs->block_size +
                           i + 0x7e00) / 64,
                          (uint64_t) - 1,
                          0, 64, (uint8_t *) records,
                          disk_info[disk_index].bs);
}

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
  do_send_key(string);
}


int is_kernel_instruction()
{
    return ((*TEMU_cpu_hflags & HF_CPL_MASK) != 3); 
}


static void tracing_taint_propagate(int nr_src,
                            taint_operand_t * src_oprnds,
                            taint_operand_t * dst_oprnd,
                            int mode)
{
  if (0==tracing_table_lookup && 2==nr_src && PROP_MODE_MOVE==mode) {
    /* if first is untainted, clear taint info of second arg */
    if (src_oprnds[0].taint==0) {
    /* clear taint info of destination */
    if (0==dst_oprnd->type) /* register */
      taintcheck_taint_register(
        dst_oprnd->addr>>2, dst_oprnd->addr&3,
        dst_oprnd->size,
        0, NULL);
    else /* memory */
      taintcheck_taint_memory(
        dst_oprnd->addr,
        dst_oprnd->size,
        0, NULL);
    eh.tp = TP_MEMREAD_INDEX;
    return;
    } else
    nr_src = 1;
  }

  /* Propagate taint, this needs to be done for all instructions */
  default_taint_propagate(nr_src, src_oprnds, dst_oprnd, mode);

  /* No need to set tp in entry header if not tracing */
  if ((!tracing_start_condition) || (tracepid == 0))
    return;

  /* No need to set tp in entry header if not 
   * tracing kernel and kernel instruction */
  if ( is_kernel_instruction() && !tracing_kernel() )
    return;

  /* Instruction propagated taint. Set tp in entry header */
  if (eh.tp == TP_NONE) eh.tp = TP_SRC;

  if(mode == PROP_MODE_MOVE) {
     /* Check if it is a memory write with tainted index */
    if ((dst_oprnd->type == 1)) {
      uint64_t a0_tainted;
      a0_tainted = taintcheck_register_check(R_A0, 0, 4, NULL);
      if (a0_tainted != 0)
        eh.tp = TP_MEMWRITE_INDEX;
    }

    if(nr_src > 1) {
      if (src_oprnds[0].taint == 0) {
        eh.tp = TP_MEMREAD_INDEX;
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
    record.numRecords = 1;
    record.taintBytes[0].source = TAINT_SOURCE_KEYBOARD_IN;
    record.taintBytes[0].origin = taint_sendkey_id;
    record.taintBytes[0].offset = keystroke_offset;
    memset(&(record.taintBytes[1]), 0,
      (MAX_NUM_TAINTBYTE_RECORDS-1)*sizeof(TaintByteRecord));

    taintcheck_taint_register(reg, 0, 1, 1, (uint8_t *) &record);
    taint_sendkey_id = 0;
    keystroke_offset++;
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

void do_tracing(uint32_t pid, const char *filename)
{
  /* if pid = 0, stop trace */
  if (0 == pid)
    tracing_stop();
  else {
    int retval = tracing_start(pid, filename);
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

void tracing_insn_begin()
{
  TAINTMEMETRY *tmp = psTMlist;
  TAINTMEMETRY *ptmp = (tmp!=NULL)?tmp->parent:NULL;
  hook_taint_record_t record;
  uint32_t paddr = 0;
  uint64_t taint = 0;

  //XXX: liulu
  //uint32_t ret_addr=0,ret_eax=0,liu1=0,liu2=0,liu3=0,liu4=0,liu5=0,liu6=0,liu7=0,liu8=0,liu9=0,liu10=0;
  //uint32_t ret1=0,ret2=0,ret3=0,ret4=0,ret5=0;


  
  /* If tracing start condition not satisified, or not tracing return */
  if ((!tracing_start_condition) || (tracepid == 0))
    return ;

  /* If not tracing kernel and kernel instruction , return */
  if ( is_kernel_instruction() && !tracing_kernel() )
    return;
  /*try to taint specific virtual memory*/

  if(tracecr3 != 0 && tracecr3 == TEMU_cpu_cr[3]){
    /*start tainting virtual memory in TAINTMEMETRY list*/
    while(tmp != NULL){
      paddr = TEMU_get_phys_addr(tmp->s_addr);  // the purpose I get paddr is just to make certain that it exists? ?      
      if(paddr == -1){
        tmp = tmp->next;
        continue;
      }
      
      memset(&record, 0, sizeof(record));
      record.source = TAINT_SOURCE_KEYBOARD_IN;
      record.origin = tmp->s_id;
      record.offset = keystroke_offset;
      //taint
      taint_mem(tmp->s_addr, tmp->s_size, (uint8_t*)&record);
      //delete entry from list
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
  
  tempconf = conf_log_external_calls; //XXX:liulu
  conf_log_external_calls = 1;
	
  if (conf_log_external_calls) {
    uint32_t eip = *TEMU_cpu_eip;
    struct names_t *names = query_name(eip);  


//XXX:XXX: liulu **********************
#if 0
	if(eip == 0x7c80a864) //getlocaltime()   2013.3.5
	{
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&getlocaltime_ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&getlocaltime_para);
	}
	if(eip == getlocaltime_ret_addr)
	{
		liululog = fopen("liulululog", "a+");
		fprintf(liululog,"getlocaltime_ret_addr = 0x%x \n",getlocaltime_ret_addr);
		fprintf(liululog,"getlocaltime_para = 0x%x \n",getlocaltime_para);
		fclose(liululog);
		
		memset(&record, 0, sizeof(record));
		
		record.source = TAINT_SOURCE_HOOKAPI;
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.
		
		taint_mem(getlocaltime_para,4, (uint8_t*)&record); //this size 4 is because one address is 4 bytes in x86 32bit.
	}
#endif

	if(eip == 0x71a25355) //gethostbyname()   2013.3.7
	{
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&gethostbyname_ret_addr);
	}
	if(eip == gethostbyname_ret_addr)
	{
		liululog = fopen("liulululog", "a+");
		fprintf(liululog,"gethostbyname_ret_addr = 0x%x \n",getlocaltime_ret_addr);
		fclose(liululog);
		
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&gethostbyname_ret_value); //the address of hostent struct.
		TEMU_read_mem(gethostbyname_ret_value,4,&addr_of_h_name); //the address of h_name
		
		memset(&record, 0, sizeof(record));
		
		record.source = TAINT_SOURCE_HOOKAPI;
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.
		
		taint_mem(addr_of_h_name,4, (uint8_t*)&record); //this size 4 is because one address is 4 bytes in x86 32bit.
	}



#if 0
	if(eip == 0x7c872a3d) //ReadConsoleA()
	{
		liululog = fopen("liulululog", "a+");
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);
		
		fprintf(liululog,"Liulu: This eip of Function:%s in Process:%s is what you want : 0x%x.\n",names->fun_name,names->mod_name,eip);
		fprintf(liululog,"Liulu: ESP value now is 0x%x \n",ret_addr);
		fprintf(liululog,"Liulu: The parameter of this function is 0x%x ,0x%x ,0x%x ,0x%x ,0x%x \n\n",liu1,liu2,liu3,liu4,liu5);			
		
		fclose(liululog);

		memset(&record, 0, sizeof(record));
		
		record.source = TAINT_SOURCE_HOOKAPI;
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.
		
		taint_mem(eip,4, (uint8_t*)&record); //this size 4 is because one address is 4 bytes in x86 32bit.		
	}


	if(eip == 0x77c1aecf) //time() will be called.	
	{
  		liululog = fopen("liulululog","a+");

		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
	
		fprintf(liululog,"Liulu: This eip of Function:%s in Process:%s is what you want!:0x%08x. \n",names->fun_name,names->mod_name,eip);
		fprintf(liululog,"Liulu: The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"Liulu: The parameter of this function is 0x%08x \n\n",liu1);	  //tt is the address of the parameter.
	
		fclose(liululog);
		
		memset(&record, 0, sizeof(record));	
		record.source = TAINT_SOURCE_HOOKAPI;
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.
	}
	
	if(eip == 0x004012c5 ) // Now the function( time() ) is finished. I will taint the parameter now.
	{
		memset(&record, 0, sizeof(record));	
		record.source = TAINT_SOURCE_HOOKAPI;
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.

		taint_mem(0x0022ff74,4,(uint8_t*)&record); //this size 4 is because one address is 4 bytes in x86 32bit.
	}



	if(eip == 0x7c92d5e0) //ZwOpenProcess 
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);

		fprintf(liululog,"ZwOpenProcess:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"ZwOpenProcess:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"ZwOpenProcess:The parameter of this function is 0x%08x,0x%08x,0x%08x,0x%08x \n\n",liu1,liu2,liu3,liu4);
		fclose(liululog);	
	}
	if(eip == 0x7c81306e) //ZwOpenProcess is over
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		
		TEMU_read_mem(liu1,4,&ret1);
		TEMU_read_mem(liu2,4,&ret2);
		TEMU_read_mem(liu3,4,&ret3);
		TEMU_read_mem(liu4,4,&ret4);

		fprintf(liululog,"ZwOPret:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"ZwOPret:The ESP now is 0x%08x \n",ret_addr);
		fprintf(liululog,"ZwOPret:The retparameter(ret1) is 0x%08x,0x%08x,0x%08x,0x%08x \n",ret1,ret2,ret3,ret4);
		fprintf(liululog,"ZwOPret:The retparameter(liu1) is 0x%08x,0x%08x,0x%08x,0x%08x \n\n",liu1,liu2,liu3,liu4);
		fclose(liululog);
	}
	if(eip == 0x7c92df90)//Zwwritevirtualmemory
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);
		fprintf(liululog,"ZwWritevirtualMemory:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"ZwWritevirtualMemory:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"ZwWritevirtualMemory:The parameter of this function is 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x \n\n",liu1,liu2,liu3,liu4,liu5);
		fclose(liululog);

	}
	if(eip == 0x7c92d190) //ZwCreateThread
	 {
	 	liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&iu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&iu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&iu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&iu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&iu5);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x18,4,&iu6);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x1c,4,&iu7);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x20,4,&iu8);

		fprintf(liululog,"ZwCreateTh:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"ZwCreateTh:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"ZwCreateTh:The retparameter is 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x \n\n",iu1,iu2,iu3,iu4,iu5,iu6,iu7,iu8);
		fclose(liululog);
	 }
	 if(eip == 0x7c819e8f) //ZwCreateThread is over 0x7c819e8f
	 {
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		
		TEMU_read_mem(iu1,4,&ret1);
		TEMU_read_mem(iu2,4,&ret2);
		TEMU_read_mem(iu3,4,&ret3);
		TEMU_read_mem(iu4,4,&ret4);
		TEMU_read_mem(iu5,4,&ret5);
		TEMU_read_mem(iu6,4,&ret6);
		TEMU_read_mem(iu7,4,&ret7);
		TEMU_read_mem(iu8,4,&ret8);

		fprintf(liululog,"ZwCTret:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"ZwCTret:The ESP now is 0x%08x \n",ret_addr);
		fprintf(liululog,"ZwCTret:The retparameter(ret1) is 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x \n",ret1,ret2,ret3,ret4,ret5,ret6,ret7,ret8);
		fprintf(liululog,"ZwCTret:The retparameter(liu1) is 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x \n\n",iu1,iu2,iu3,iu4,iu5,iu6,iu7,iu8);
		fclose(liululog);
	 }


	if(eip == 0x009d97df) //WriteProcessMemory 's return add
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);

		fprintf(liululog,"WPMret:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"WPMret:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"WPMret:The parameter of this function is 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x \n\n",liu1,liu2,liu3,liu4,liu5);
		fclose(liululog);
	}
#endif

//*********************************************************************************************************************************************************
#if 0
	if(eip == 0x7c8309d1) //kernel32.dll::OpenProcess
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		openpro_ret_addr = ret_addr;
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&OpenPid);

		//fprintf(liululog,"OpenProcess:The EAX now is 0x%08x \n",ret_eax);
		//fprintf(liululog,"OpenProcess:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"OpenProcess[0x%08x,0x%08x,0x%08x(Pid)] \n",liu1,liu2,OpenPid);
		fclose(liululog);
	}
	if(eip == openpro_ret_addr) //openprocess is over
	{
		liululog = fopen("liulululog","a+");
		//fprintf(liululog,"OpenProcess is over:The Temu_eax now is 0x%08x(PHandle) \n\n",TEMU_cpu_regs[R_EAX]);

		if( TEMU_cpu_regs[R_EAX] != 0 )
			{
				fprintf(liululog,"OpenProcess Succeed. PHandle:0x%08x. ",TEMU_cpu_regs[R_EAX]);
				fprintf(liululog," ProcessID:%d. \n\n",OpenPid);
			}
		else	
			fprintf(liululog,"OpenProcess Failed. \n\n");		


		fclose(liululog);
	}
	
	

	if(eip == 0x7c809b02) //VirtualAllocEx 0x7c809b02
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		virAlloc_ret_addr = ret_addr;
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);

		//fprintf(liululog,"VirtualAllocEx:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"VirtualAllocEx[0x%08x(PHandle),0x%08x,0x%08x(Size),0x%08x,0x%08x] \n",liu1,liu2,liu3,liu4,liu5);
		fclose(liululog);
	}
	if(eip == virAlloc_ret_addr) //VirtualAllocEx is over
	{
		liululog = fopen("liulululog","a+");
		//fprintf(liululog,"VirtualAllocEx is over:The Temu_eax now is 0x%08x(FirstAddr) \n\n",TEMU_cpu_regs[R_EAX]);
		if( TEMU_cpu_regs[R_EAX] != 0 )
			fprintf(liululog,"VirtualAllocEx Succeed. AllocFirstAddr:0x%08x \n\n",TEMU_cpu_regs[R_EAX]);
		else	
			fprintf(liululog,"VirtualAllocEx Failed. \n\n");		
		fclose(liululog);	
	}


	if(eip == 0x7c802213) //writeProcessmemory 0x7c802213
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		writepro_ret_addr = ret_addr;
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);

		//fprintf(liululog,"WriteProcessMemory:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"WriteProcessMemory[0x%08x(PHandle),0x%08x(FirstAddr),0x%08x(buf),0x%08x(Size),0x%08x] \n",liu1,liu2,liu3,liu4,liu5);
		fclose(liululog);
	}
	if(eip == writepro_ret_addr) //writeProcessmemory is over
	{
		liululog = fopen("liulululog","a+");
		if( TEMU_cpu_regs[R_EAX] != 0 )
			fprintf(liululog,"WriteProcessMemory Succeed. \n\n");
		else	
			fprintf(liululog,"WriteProcessMemory Failed. \n\n");
		fclose(liululog);	
	}
	

	if(eip == 0x7c8104bc) //CreateRemoteThread 0x7c8104bc
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x18,4,&liu6);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x1c,4,&liu7);
		//fprintf(liululog,"CreateRemoteThread:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"CreateRemoteThread[0x%08x(PHandle),0x%08x,0x%08x,0x%08x(FirstAddr),0x%08x(Para),0x%08x,0x%08x] \n\n",liu1,liu2,liu3,liu4,liu5,liu6,liu7);
		fclose(liululog);
	}



	/*	if(eip == 0x7c809bd7) // kernel32.dll::CloseHandle @ EIP: 0x7c809bd7
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);

		//fprintf(liululog,"OpenProcess:The EAX now is 0x%08x \n",ret_eax);
		fprintf(liululog,"CloseHandle:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"CloseHandle:The parameter: 0x%08x(handle)\n\n",liu1);
		fclose(liululog);
	} */
#endif

	if(eip == 0x7d685bf0) //shell32.dll::ShellExecuteW @ EIP: 0x7d685bf0
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&ret_eax);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x14,4,&liu5);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x18,4,&liu6);
		
		fprintf(liululog,"ShellExecuteW[0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x] \n",liu1,liu2,liu3,liu4,liu5,liu6);
		fprintf(liululog,"ShellExecuteW:The para3(filepath) is ");
		
		TEMU_read_mem(liu3,4,&shellpath[0]);
		TEMU_read_mem(liu3+0x4,4,&shellpath[1]);
		TEMU_read_mem(liu3+0x8,4,&shellpath[2]);
		TEMU_read_mem(liu3+0xc,4,&shellpath[3]);
		
		fprintf(liululog,"%08x ",shellpath[0]);
		fprintf(liululog,"%08x ",shellpath[1]);
		fprintf(liululog,"%08x ",shellpath[2]);
		fprintf(liululog,"%08x ",shellpath[3]);

		fprintf(liululog,"\n\n\n\n");

#if 0
		//make taint!
		memset(&record, 0, sizeof(record));	
		record.source = TAINT_SOURCE_NETWORK_OUT; //sure???
		record.origin = 0x88888888; //let is spec
		record.offset = 0; //now 0, maybe change it later.
		taint_mem(liu3,4,(uint8_t*)&record);  //now just taint "cm"
#endif

#if 0	
		//change the scripts  cmd.exe:006d0063 002e0064 00780065 00000065
		cmd1 = 0x006d0063;
		cmd2 = 0x002e0064;
		cmd3 = 0x00780065;
		cmd4 = 0x00000065;
		buf1 = &cmd1;
		buf2 = &cmd2;
		buf3 = &cmd3;
		buf4 = &cmd4;
		TEMU_write_mem(liu3,4,buf1);
		TEMU_write_mem(liu3+0x4,4,buf2);
		TEMU_write_mem(liu3+0x8,4,buf3);
		TEMU_write_mem(liu3+0xc,4,buf4);	
#endif


#if 0
		//9.24 Get the taint!
		//uint64_t get_mem_taint(uint32_t vaddr, uint32_t size, uint8_t *rec)
		memset(&record, 0, sizeof(record));
		get_mem_taint(liu3,1,(uint8_t*)&record);
		fprintf(liululog,"Source:0x%08x  Origin:0x%08x  Offset:0x%08x \n\n ",record.source,record.origin,record.offset);
		
#endif
		fclose(liululog);
	}


#if 0
	if(eip == 0x71a42e70) //wsock32.dll::recv @ EIP: 0x71a42e70
	{
		liululog = fopen("liulululog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&recv_ret_addr);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&recv_buf_addr);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu4);

		TEMU_read_mem(liu1,4,&liu5);
		fprintf(liululog,"recv:The return address is 0x%08x \n",ret_addr);
		fprintf(liululog,"recv:The parameter:0x%08x(Socket),0x%08x(*buf),0x%08x(Length),0x%08x(Flags) \n",liu1,recv_buf_addr,liu3,liu4);
		fprintf(liululog,"recv:The parameter:0x%08x(Socket!) \n\n\n",liu5);
		fclose(liululog);
	}


	if(eip == recv_ret_addr) //recv() is over   		  XXX:recv_ret_addr is only used by recv().
	{
		liululog = fopen("liulululog","a+");
		
		TEMU_read_mem(recv_buf_addr,4,&recv_buf[0]);
		TEMU_read_mem(recv_buf_addr+0x4,4,&recv_buf[1]);
		TEMU_read_mem(recv_buf_addr+0x8,4,&recv_buf[2]);
		TEMU_read_mem(recv_buf_addr+0xc,4,&recv_buf[3]);
		TEMU_read_mem(recv_buf_addr+0x10,4,&recv_buf[4]);

		fprintf(liululog,"recv_buf:0x%08x 0x%08x 0x%08x 0x%08x 0x%08x \n\n\n",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3],recv_buf[4]);
		
		fclose(liululog);
	}

#endif

#if 0
	if(eip == 0x7c80aa26) //kernel32.dll::lstrcmpiW @ EIP: 0x7c80aa26
	{
		strcmplog = fopen("strcmplog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		
		for(p=0;p<30;p++)
		{
			TEMU_read_mem(liu1+4*p,4,&cmpstr1[p]);
			TEMU_read_mem(liu2+4*p,4,&cmpstr2[p]);
		}

		//make One uint32_t to Two char.Then I can read it directly.
		for(p=0,q=0;p<15;p++,q++)
		{
			realstr1[q] = (char)(cmpstr1[p] & 0x000000ff);
			if((realstr1[q] < ' ') || (realstr1[q] > '~' ))
				realstr1[q] = ' ';
			q++;
			realstr1[q] = (char)(( cmpstr1[p] >> 16 ) & 0x000000ff);
			if((realstr1[q] < ' ') || (realstr1[q] > '~' ))
				realstr1[q] = ' ';
		}
		
		for(p=0,q=0;p<15;p++,q++)
		{
			realstr2[q] = (char)(cmpstr2[p] & 0x000000ff);
			if((realstr2[q] < ' ') || (realstr2[q] > '~') )
				realstr2[q] = ' ';	
			q++;
			realstr2[q] = (char)(( cmpstr2[p] >> 16 ) & 0x000000ff);
			if((realstr2[q] < ' ') || (realstr2[q] > '~' ))
				realstr2[q] = ' ';
		}
		
		//fprintf(liululog,"lstrcmpiW:The return address is 0x%08x \n",ret_addr);
		//fprintf(liululog,"lstrcmpiW:The parameter:0x%08x(lprt1),0x%08x(*lpt2) \n",liu1,liu2);
		if(realstr1[0] != ' ')
		{
			fprintf(strcmplog,"lstrcmpiW:String(1)=\"");
			for(p=0;p<30;p++)
			{
				if( realstr1[p] == ' ' )
					break;
				fprintf(strcmplog,"%c",realstr1[p]);
			}
			fprintf(strcmplog,"\"\n");


			fprintf(strcmplog,"lstrcmpiW:String(2)=\"");
			for(p=0;p<30;p++)
			{
				if( realstr2[p] == ' ' )
					break;
				fprintf(strcmplog,"%c",realstr2[p]);
			}
			fprintf(strcmplog,"\"\n\n");
		
		}

#if 0
		//TODO:change the script to XXX when the script is  something else
		//0x00730075 0x00720065 0x0065005f 0x00650078 0x00750063 0x00650074  user_execute
		//0x0073006f 0x0073005f 0x00750068 0x00640074 0x0077006f 0x0000006e  os_Shutdown
		//0x0073006f 0x0072005f 0x00620065 0x006f006f 0x00000074 os_reboot
		//0x00730075 0x00720065 0x0068005f 0x006d006f 0x00700065 0x00670061 0x005f0065 0x00650073 0x00000074 user_homepage_set
		if( (cmpstr1[0] == 0x00730075) && (cmpstr1[1] == 0x00720065) && (cmpstr1[2] == 0x0068005f)
			&& (cmpstr1[3] == 0x006d006f) && (cmpstr1[4] == 0x00700065) && (cmpstr1[5] == 0x00670061)
			&& (cmpstr1[6] == 0x005f0065) && (cmpstr1[7] == 0x00650073) )	 //user_homepage_set
		{
			//change to  user_execute
			cmpstr1[0] = 0x00730075;
			cmpstr1[1] = 0x00720065;
			cmpstr1[2] = 0x0065005f;
			cmpstr1[3] = 0x00650078;
			cmpstr1[4] = 0x00750063;
			cmpstr1[5] = 0x00650074;
			cmpstr1[6] = 0x00000000;
			 //XXX:Attention! Make the last asciii be "0000"(NULL)!!

			buf1 = &cmpstr1[0];
			buf2 = &cmpstr1[1];
			buf3 = &cmpstr1[2];
			buf4 = &cmpstr1[3];
			buf5 = &cmpstr1[4];
			buf6 = &cmpstr1[5];
			buf7 = &cmpstr1[6];

			TEMU_write_mem(liu1,4,buf1);
			TEMU_write_mem(liu1+0x4,4,buf2);
			TEMU_write_mem(liu1+0x8,4,buf3);
			TEMU_write_mem(liu1+0xc,4,buf4);
			TEMU_write_mem(liu1+0x10,4,buf5);
			TEMU_write_mem(liu1+0x14,4,buf6);
			TEMU_write_mem(liu1+0x18,4,buf7);
		}

#endif
		fclose(strcmplog);	
	}

	//0x77c17730 strcmp -11.15
	if(eip == 0x77c17730)
	{
		strcmplog = fopen("strcmplog","a+");
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&ret_addr);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);

		fprintf(strcmplog,"strcmp:String(1)= ");
		for(p=0;p < 20;p++)
		{
			TEMU_read_mem(liu1+p,1,&cmpstr1[p]);		
			if((cmpstr1[p] < ' ') || (cmpstr1[p] > '~' ))
				break;
					
			fprintf(strcmplog,"%c",cmpstr1[p]);		
		}
		fprintf(strcmplog,"\n");
		
		fprintf(strcmplog,"strcmp:String(2)= ");
		for(p=0;p<20;p++)
		{
			TEMU_read_mem(liu2+p,1,&cmpstr2[p]);
			if((cmpstr2[p] < ' ') || (cmpstr2[p] > '~' ))
				break;
			fprintf(strcmplog,"%c",cmpstr2[p]);

		}
		fprintf(strcmplog,"\n\n");
		fclose(strcmplog);	
	}

	
#endif



#if 0
	if((eip > 0x018c0000) && (eip < 0x018c0000 + 0x00007000))
	{
		liululog = fopen("liulululog","a+");
		
		fprintf(liululog,"~~~~~~~~~~~~~~~~~~~~~~The content @0x%08x is running. \n",eip);
		
		fclose(liululog);
	}
#endif

#if 0
	//get the injected data.
	//run it only once
	if(flag_917 == 0)
	{
		
		flag_917 = 1;
		injectlog = fopen("injectlog","a+");
		injectbitlog = fopen("injectbitlog","a+");
	 	for(i_917 = 0;i_917 < 160000;i_917++)
  			buffer[i_917] = 0x22;

              	//inject_addr = 0x009e0000;	//cftmon.exe
              	//inject_addr = 0x00d00000;	//explorer.exe
		//inject_addr = 0x00d08088; //first fuction
		inject_addr = 0x00d00000;	//explorer.exe-10.18


              	inject_addr_label = inject_addr;
                fprintf(injectlog,"0x%08x: ",inject_addr_label);
	 	for(i_917 = 0;i_917 < 160000;i_917++)
	 	{	
	 		TEMU_read_mem(inject_addr,1,&buffer[i_917]); //explorer.exe
	 		inject_addr++;

			fwrite(&buffer[i_917], sizeof(buffer[i_917]), 1, injectbitlog);
	 		
	 		fprintf(injectlog,"%02x ",buffer[i_917]);
	 		if((i_917 + 1) % 16 == 0)
	 		{
				fprintf(injectlog,"\n");
				inject_addr_label = inject_addr_label + 0x10;
				fprintf(injectlog,"0x%08x: ",inject_addr_label);
	 		}
	 	}
		fprintf(injectlog,"\n\n");
		fclose(injectlog);
		fclose(injectbitlog);
	}
#endif

//*************************************************************************************************************
//liulutest-929

#if 0
	testlog = fopen("testlog","a+");
	if(eip == 0x7c93e40f) //wcsncmp @ EIP: 0x7c93e40f
	{
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&wsncmp_ret_addr);
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);

		TEMU_read_mem(liu1,4,&liu4);
		TEMU_read_mem(liu2,4,&liu5);
		
		fprintf(testlog,"wcsncmp:The return address is 0x%08x \n",ret_addr);
		fprintf(testlog,"wcsncmp:The parameter:0x%08x(str1),0x%08x(str2),0x%08x(count) \n",liu4,liu5,liu3);
	}
	if(eip == wsncmp_ret_addr)
	{
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&liu6); 
		fprintf(testlog,"wcsncmp:Return bool:0x%08x \n\n\n",liu6);		
	}


	if(eip == 0x7c80d107) //CompareStringA @ EIP: 0x7c80d107
	{
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&cmparestringA_ret_addr); 
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x10,4,&liu2);

		TEMU_read_mem(liu1,4,&liu3);
		TEMU_read_mem(liu2,4,&liu4);
		
		fprintf(testlog,"CompareStringA:The return address is 0x%08x \n",ret_addr);
		fprintf(testlog,"CompareStringA:The parameter:0x%08x(str1),0x%08x(str2)\n",liu3,liu4);
	}
	if(eip == cmparestringA_ret_addr)
	{
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&liu6); 
		fprintf(testlog,"CompareStringA:Return bool:0x%08x \n\n\n",liu6);		
		
	}



	if(eip == 0x7c932e9b) //RtlEqualUnicodeString @ EIP: 0x7c932e9b
	{
		TEMU_read_mem(TEMU_cpu_regs[R_ESP],4,&rtlequal_ret_addr); 
		
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x4,4,&liu1);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0x8,4,&liu2);
		TEMU_read_mem(TEMU_cpu_regs[R_ESP]+0xc,4,&liu3);

		TEMU_read_mem(liu1,4,&liu4);
		TEMU_read_mem(liu2,4,&liu5);
		
		fprintf(testlog,"RtlEqualUnicodeString:The return address is 0x%08x \n",ret_addr);
		fprintf(testlog,"RtlEqualUnicodeString:The parameter:0x%08x(str1),0x%08x(str2),0x%08x \n",liu4,liu5,liu3);
	}
	if(eip == rtlequal_ret_addr)
	{
		TEMU_read_mem(TEMU_cpu_regs[R_EAX],4,&liu6); 
		fprintf(testlog,"RtlEqualUnicodeString:Return bool:0x%08x \n\n\n",liu6);		
	}


	
	fclose(testlog);
#endif




//*******************************************************************************************************************************

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

  }

	conf_log_external_calls = tempconf;

  CanTraceRdtsc = 0;


  /* Disassemble the instruction */
  decode_address(*TEMU_cpu_eip, &eh, skip_taint_info);  //XXX:




#ifdef TAINT_LOOP_IVS
  /* If not tracing, skip */
  if (tracepid != 0)
      taint_loop_ivs();
#endif

#ifdef INSN_INFO
  savedeip = *TEMU_cpu_eip;
#endif

}

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

  /* Taint the network */
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
  /* taint rdtsc insn */
  {"taint_rdtsc", "", do_taint_rdtsc, "", "taint rdtsc instruction"},
  /* taint PEB memory */
  {"taint_peb", "", do_taint_peb, "", "taint PEB memory"},
  /*new operation for taint memory*/
  {"taint_mem", "ssi", do_taint_memory,
   "addr size id", "taint memory in the process"},
  /* operations to set taint source */
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
  tracing_interface.after_loadvm = tracing_after_loadvm;
  tracing_interface.cjmp = tracing_cjmp;
  removeproc_notify = procexit_detection;

  tracing_init ();
  return &tracing_interface;
}
