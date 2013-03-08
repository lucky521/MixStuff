#ifndef _NETWORK_H_
#define _NETWORK_H_

/* Functions */
void do_taint_nic(int state);
void print_nic_filter ();
int update_nic_filter (const char *filter_str, const char *value_str);
void tracing_nic_recv(uint8_t * buf, int size, int index, int start, int stop);
void tracing_nic_send(uint32_t addr, int size, uint8_t * buf);

#endif // _NETWORK_H_

