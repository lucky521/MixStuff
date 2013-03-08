#ifndef _CONF_H_
#define _CONF_H_
#include "tracecap.h"
/* llconf */
#include <llconf/modules.h>
#include <llconf/ini.h>
#include <llconf/nodes.h>
#include <llconf/entry.h>


/* External Variables */
extern int conf_trace_only_after_first_taint;
extern int conf_log_external_calls;
extern int tempconf;

extern int conf_write_ops_at_insn_end;
extern int conf_save_state_at_trace_stop;
extern int tracing_table_lookup;
char hook_dirname[256];
char hook_plugins_filename[256];
char ini_main_default_filename[256];


/* Functions */
void check_ini(char *path_ini);
void print_conf_vars();

void set_ignore_dns(int state);
int tracing_ignore_dns();

void set_tainted_only(int state);
int tracing_tainted_only();

void set_kernel_all(int state);
int tracing_kernel_all();

void set_kernel_tainted(int state);
int tracing_kernel_tainted();

void set_kernel_partial(int state);
int tracing_kernel_partial();

int  tracing_kernel();


#endif // _CONF_H_

