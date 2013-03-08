#ifndef _TRACECAP_H_
#define _TRACECAP_H_

#include <stdio.h>
#include <inttypes.h>
#include <sys/user.h>
#include "trace.h"
#include "../shared/hookapi.h"
#include "TEMU_lib.h"
#include "conf.h"


#undef INLINE
#define TAINT_LOOP_IVS

/* Some configuration options that we don't foresee people to change 
 * Thus, they are not part of the ini configuration file */
#define PRINT_FUNCTION_MAP 1


/* Exit codes */
#define EXIT_NORMAL 1
#define EXIT_KILL_SIGNAL 13
#define EXIT_KILL_MSG 13
#define EXIT_DETECT_TAINTEIP 21
#define EXIT_DETECT_EXCEPTION 22
#define EXIT_DETECT_NULLPTR 23
#define EXIT_DETECT_PROCESSEXIT 24

/* External Variables */

extern FILE *liululog;
extern FILE *injectlog; //10.17
extern FILE *injectbitlog;
extern FILE *strcmplog;
extern FILE *testlog;
//liulu

extern FILE *tracelog;
extern FILE *tracenetlog;
extern FILE *tracehooklog;
extern FILE *calllog;
extern FILE *alloclog;
extern uint32_t tracepid;
extern uint32_t tracecr3;
extern EntryHeader eh;

extern int skip_taint_info;   // If !=0, operands will have empty taint records
extern char *tracename_p;

/* Functions */
void do_taint_sendkey(const char *string, int id);
void do_taint_file(char *filename, int dev_index, uint32_t taint_id);
void do_linux_ps();
void do_tracing(uint32_t pid, const char *filename);
void do_tracing_by_name(const char *progname, const char *filename);
void do_save_state(uint32_t pid, uint32_t address, const char *filename);
void do_guest_modules(uint32_t pid);
void do_add_iv_eip(uint32_t eip);
void do_clean_iv_eips();

void save_registers(struct user_regs_struct *regs);
int init_state_dump(uint32_t pid, uint32_t addr, const char *filename);
int save_state_init(uint32_t pid, uint32_t addr, const char *filename);
/* static void dump_memory(uint32_t *pstart, int *plast_prot,  */
/*      uint32_t end, int prot); */
int tracing_start(uint32_t pid, const char *filename);
void tracing_stop();
void taint_loop_ivs();

#endif // _TRACECAP_H_
