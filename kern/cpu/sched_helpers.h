/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_SCHED_HELPERS_H
#define FOS_KERN_SCHED_HELPERS_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/environment_definitions.h>

/*2023*/
/********* for BSD Priority Scheduler *************/
int env_get_nice(struct Env* e) ;
void env_set_nice(struct Env* e, int nice_value) ;
int env_get_recent_cpu(struct Env* e) ;
int get_load_average() ;
/********* for BSD Priority Scheduler *************/

/*2024*/
void env_set_priority(int envID, int priority);
void sched_set_starv_thresh(uint32 starvThresh);


void sched_insert_ready0(struct Env* env);
void sched_insert_ready(struct Env* env);
void sched_remove_ready(struct Env* env);
void sched_insert_new(struct Env* env);
void sched_remove_new(struct Env* env);
void sched_insert_exit(struct Env* env);
void sched_remove_exit(struct Env* env);

//2015:
void sched_new_env(struct Env* e);
void sched_run_env(uint32 envId);
//void sched_new_env(uint32 envId);
void sched_exit_env(uint32 envId);
void sched_kill_env(uint32 envId);
void sched_kill_all();
void sched_exit_all_ready_envs();
void sched_print_all();
void sched_run_all();
void sched_delete_ready_queues() ;

//2018:
//Declaration of helper functions to deal with the env queues
void init_queue(struct Env_Queue* queue);
int queue_size(struct Env_Queue* queue);
void enqueue(struct Env_Queue* queue, struct Env* env);
struct Env* dequeue(struct Env_Queue* queue);
struct Env* find_env_in_queue(struct Env_Queue* queue, uint32 envID);
void remove_from_queue(struct Env_Queue* queue, struct Env* e);
void sleep_sem(struct Env_Queue* queue);
void sleep_sem_sig(struct Env_Queue* queue) ;
void release_spin();
void acquire_spin();
#endif	// !FOS_KERN_SCHED_HELPERS_H
