#ifndef _PROCESS_H
#define _PROCESS_H


int master_init();
int master_cycle();
int master_start_worker();
int master_spawn_worker();


int worker_init();
int worker_cycle();

#endif
