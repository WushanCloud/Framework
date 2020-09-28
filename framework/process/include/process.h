#ifndef __FUNC_H__
#define __FUNC_H__

#include "config.h"

void init_proctitle();
void set_proctitle(const char* name);

void start_worker_process(int nums, int type);
void worker_process_cycle(int inum, const char* procname);

void master_process_cycle();
void spawn_process(int inum, const char* procname, int type);

int set_daemon();

#endif // __FUNC_H__
