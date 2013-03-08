#include "config.h"
#include <stdio.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "tracecap.h"
#include "bswap.h"
#include "TEMU_lib.h"
#include "shared/hookapi.h"
#include "shared/procmod.h"
#include "shared/read_linux.h"
#include "shared/hooks/function_map.h"
#include "shared/hooks/hook_plugin_loader.h"
#include "trace.h"

#if TRACE_ENABLED


FILE *liululog = 0; //liulu
FILE *injectlog = 0; //10.17
FILE *injectbitlog = 0;
FILE *strcmplog = 0; //liulu
FILE *testlog = 0; //liulu

FILE *tracelog = 0;
FILE *tracenetlog = 0;
FILE *tracehooklog = 0;
FILE *calllog = 0;
FILE *statelog = 0;
FILE *alloclog = 0;
uint32_t tracepid = 0;
uint32_t tracecr3 = 0;
uint32_t dump_pc_start = 0;
int skip_taint_info = 0;

uint32_t statepid = 0;
uint32_t statecr3 = 0;
uint32_t stateaddr = 0;
uint32_t stateaddr_hook_handle = 0;

/* Filename for functions file */
char functionsname[128]= "";

/* Filename for trace file */
char tracename[128]= "";
char *tracename_p = tracename;




/* Start usage */
struct rusage startUsage;

/* Entry header */
EntryHeader eh;


void save_registers(struct user_regs_struct *regs)
{
  regs->ebx = TEMU_cpu_regs[R_EBX];
  regs->ecx = TEMU_cpu_regs[R_ECX];
  regs->edx = TEMU_cpu_regs[R_EDX];
  regs->esi = TEMU_cpu_regs[R_ESI];
  regs->edi = TEMU_cpu_regs[R_EDI];
  regs->ebp = TEMU_cpu_regs[R_EBP];
  regs->eax = TEMU_cpu_regs[R_EAX];
  regs->xds = TEMU_cpu_segs[R_DS].selector;
  regs->xes = TEMU_cpu_segs[R_ES].selector;
  regs->xfs = TEMU_cpu_segs[R_FS].selector;
  regs->xgs = TEMU_cpu_segs[R_GS].selector;
  regs->orig_eax = 0;           //? Do we need to remember the call number
  regs->eip = *TEMU_cpu_eip;
  regs->xcs = TEMU_cpu_segs[R_CS].selector;
  regs->eflags = *TEMU_cpu_eflags;
  regs->esp = TEMU_cpu_regs[R_ESP];
  regs->xss = TEMU_cpu_segs[R_SS].selector;
}


typedef struct {
  unsigned int begin, end;
} map_t;

static void dump_memory(uint32_t * pstart, int *plast_prot,
                        uint32_t end, int prot)
{
  int prot1;
  prot1 = *plast_prot;
  if (prot != prot1) {
    if (*pstart != -1) {
      uint32_t start;
      int len;

      start = *pstart;
      len = end - start;

      map_t range;
      range.begin = start;
      range.end = end;

      // hack to detect kernel memory
      unsigned long kerneladdr;

      if (taskaddr) {
        // linux
        kerneladdr = 0xC0000000;
      } else {
        // windows
        kerneladdr = 0x80000000;
      }

      if (range.begin >= kerneladdr)
        return;

      fwrite(&range, sizeof(range), 1, statelog);

      char buf[4096];
      while (len > 4096) {
        TEMU_read_mem(start, 4096, buf);
        start += 4096;
        len -= 4096;
        fwrite(buf, 1, 4096, statelog);
      }
      TEMU_read_mem(start, len, buf);
      fwrite(buf, 1, len, statelog);
      fflush(statelog);
    }
    if (prot != 0)
      *pstart = end;
    else
      *pstart = -1;
    *plast_prot = prot;
  }
}

static void traverse_pages(void)
{
  int l1, l2, prot, last_prot;
  uint32_t pgd, pde, pte, start, end;


  if (!(TEMU_cpu_cr[0] & CR0_PG_MASK)) {
    term_printf("PG disabled\n");
    return;
  }
  pgd = TEMU_cpu_cr[3] & ~0xfff;
  last_prot = 0;
  start = -1;
  for (l1 = 0; l1 < 1024; l1++) {
    cpu_physical_memory_rw(pgd + l1 * 4, (uint8_t *) & pde, 4, 0);
    pde = le32_to_cpu(pde);
    end = l1 << 22;

    //check whether it is kernel memory
    //    if (!(pde & PG_USER_MASK))
    //        continue;


    if (pde & PG_PRESENT_MASK) {
      if ((pde & PG_PSE_MASK) && (TEMU_cpu_cr[4] & CR4_PSE_MASK)) {
        prot = pde & (PG_USER_MASK | PG_RW_MASK | PG_PRESENT_MASK);
        dump_memory(&start, &last_prot, end, prot);
      }
      else {
        for (l2 = 0; l2 < 1024; l2++) {
          cpu_physical_memory_rw((pde & ~0xfff) + l2 * 4,
                                   (uint8_t *) & pte, 4, 0);
          pte = le32_to_cpu(pte);
          end = (l1 << 22) + (l2 << 12);
          if (pte & PG_PRESENT_MASK) {
            prot = pte & (PG_USER_MASK | PG_RW_MASK | PG_PRESENT_MASK);
          }
          else {
            prot = 0;
          }
          dump_memory(&start, &last_prot, end, prot);
        }
      }
    }
    else {
      prot = 0;
      dump_memory(&start, &last_prot, end, prot);
    }
  }
}

int save_state(void *opaque)
{
  if (TEMU_cpu_cr[3] != statecr3)
    return 0;

  //vm_stop(EXCP_INTERRUPT);
  term_printf("Saving state: %x, %d\n", stateaddr, statepid);


  hookapi_remove_hook(stateaddr_hook_handle);

  if (statelog) {
    term_printf("Saving state CR3: %x v.s. %x\n",
      TEMU_cpu_cr[3], statecr3);
    /* save registers */

    struct user_regs_struct regs;
    save_registers(&regs);
    fwrite(&regs, sizeof(regs), 1, statelog);
    fflush(statelog);

    /* save memories */
    traverse_pages();
    fclose(statelog);
    statelog = 0;
  }
  return 0;
}

int save_state_init(uint32_t pid, uint32_t addr, const char *filename)
{
  statepid = pid;
  statecr3 = find_cr3(pid);
  stateaddr = addr;

  statelog = fopen(filename, "w");
  if (0 == statelog)
    return -1;

  //term_printf("Hooking address: 0x%x\n", stateaddr);
  stateaddr_hook_handle = hookapi_hook_function(0, stateaddr, save_state, NULL, 0);
  return 0;

}


int tracing_start(uint32_t pid, const char *filename)
{
#ifdef INSN_INFO
  char infoname[128];
  if (infolog)
    fclose(infolog);
  snprintf(infoname, 128, "%s.log", filename);
  infolog = fopen(infoname, "w");
#endif
	

	
  if (conf_log_external_calls) {
    trace_do_not_write = 1;  
  }



  /* Initialize disassembler */
  xed2_init();

  strncpy(tracename, filename, 128);

  if (tracelog)
    fclose(tracelog);
  if (tracenetlog)
    fclose(tracenetlog);



  tracelog = fopen(filename, "w");
  setvbuf(tracelog, tfilebuf, _IOFBF, FILEBUFSIZE);
  if (0 == tracelog) {
    tracepid = 0;
    tracecr3 = 0;
    return -1;
  }

  char netname[128];
  snprintf(netname, 128, "%s.netlog", filename);
  tracenetlog = fopen(netname, "w");
  if (0 == tracenetlog) {
    tracepid = 0;
    tracecr3 = 0;
    return -1;
  }
  else {
    fprintf(tracenetlog, "Flow       Off  Data\n");
    fflush(tracenetlog);
  }

  // Set name for functions file
  snprintf(functionsname, 128, "%s.functions", filename);
  
	tempconf=conf_log_external_calls; //liulu
	conf_log_external_calls = 1;
  if (conf_log_external_calls) {
    char callname[128];
    if (calllog)
      fclose(calllog);
    snprintf(callname, 128, "%s.calls", filename);
    calllog = fopen(callname, "w");
    setvbuf(calllog, cfilebuf, _IOFBF, FILEBUFSIZE);
    if (0 == calllog) {
      tracepid = 0;
      tracecr3 = 0;
      return -1;
    }
  }
   conf_log_external_calls = tempconf;

  tracepid = pid;
  tracecr3 = find_cr3(pid);
  if (0 == tracecr3) {
    term_printf("CR3 for PID %d not found. Tracing all processes!\n",pid);
  }
  term_printf("PID: %d CR3: 0x%08x\n",tracepid, tracecr3);


  /* Initialize hooks only for this process */
  temu_plugin->monitored_cr3 = tracecr3;

  if (conf_save_state_at_trace_stop) {
    char statename[128];
    snprintf(statename, 128, "%s.state", filename);
    save_state_init(tracepid, 0, statename);
  }

  /* Get system start usage */
  if (getrusage(RUSAGE_SELF, &startUsage) != 0)
    term_printf ("Could not get start usage\n");

  return 0;
}

void term_printf(const char *fmt, ...);

void tracing_stop()
{
  term_printf("Stop tracing process %d\n", tracepid);
  print_stats();

  /* Get system stop usage */
  struct rusage stopUsage;
  if (getrusage(RUSAGE_SELF, &stopUsage) == 0) {
    double startUT = (double)startUsage.ru_utime.tv_sec +
                    (double)startUsage.ru_utime.tv_usec / 1e6;
    double startST = (double)startUsage.ru_stime.tv_sec +
                    (double)startUsage.ru_stime.tv_usec / 1e6;
    double stopUT = (double)stopUsage.ru_utime.tv_sec +
                    (double)stopUsage.ru_utime.tv_usec / 1e6;
    double stopST = (double)stopUsage.ru_stime.tv_sec +
                    (double)stopUsage.ru_stime.tv_usec / 1e6;

    double userProcessTime = (stopUT - startUT);
    double systemProcessTime = (stopST - startST);
    double processTime =  userProcessTime + systemProcessTime;

    term_printf ("Processing time: %g U: %g S: %g\n",
      processTime, userProcessTime, systemProcessTime);
  }
  else {
    term_printf ("Could not get usage\n");
  }


  tracepid = 0;
  if (tracelog) {
    fclose(tracelog);
    tracelog = 0;
  }

  if (tracenetlog) {
    fclose(tracenetlog);
    tracenetlog = 0;
  }

  if (tracehooklog) {
    fclose(tracehooklog);
    tracehooklog = 0;
  }

  if (alloclog) {
    fclose(alloclog);
    alloclog = 0;
  }

  // Clear received_data flag
  received_tainted_data = 0;

// Print file with all functions offsets
#if PRINT_FUNCTION_MAP
  map_to_file(functionsname);
#endif

	tempconf = conf_log_external_calls;
	conf_log_external_calls = 1; //liulu
  if (conf_log_external_calls) {
    if (calllog) {
      fclose(calllog);
      calllog = 0;
    }
  }
  conf_log_external_calls = tempconf;

#ifdef INSN_INFO
  if (infolog) {
    fclose(infolog);
    infolog = 0;
  }
#endif

  if (conf_save_state_at_trace_stop) {
    save_state(NULL);
  }

  /* Do not unload hooks, it'd crash emulation */

}

#endif                          //TRACE_ENABLED
