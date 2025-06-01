/*
 * channel.c
 *
 *  Created on: Sep 22, 2024
 *      Author: HP
 */
#include "channel.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <inc/string.h>
#include <inc/disk.h>

//===============================
// 1) INITIALIZE THE CHANNEL:
//===============================
// initialize its lock & queue
void init_channel(struct Channel *chan, char *name)
{
	strcpy(chan->name, name);
	init_queue(&(chan->queue));
}

//===============================
// 2) SLEEP ON A GIVEN CHANNEL:
//===============================
// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
// Ref: xv6-x86 OS code
void sleep(struct Channel *chan, struct spinlock *lk) {
	//TODO: [PROJECT'24.MS1 - #10] [4] LOCKS - sleep
		//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//	panic("sleep is not implemented yet");
	//Your Code is Here...

	  struct Env *currenttt_prockk_ahna_fiha = get_cpu_proc();  // Get the current running process

	    if (!currenttt_prockk_ahna_fiha || currenttt_prockk_ahna_fiha->env_status != ENV_RUNNING) {
	        return;
	    }

	    acquire_spinlock(&ProcessQueues.qlock);
	    release_spinlock(lk);

	    enqueue(&chan->queue, currenttt_prockk_ahna_fiha);
	    // bn8ir al status l blocked
	    currenttt_prockk_ahna_fiha->env_status = ENV_BLOCKED;

	    sched();
	    release_spinlock(&ProcessQueues.qlock);
	    acquire_spinlock(lk);

}



//==================================================
// 3) WAKEUP ONE BLOCKED PROCESS ON A GIVEN CHANNEL:
//==================================================
// Wake up ONE process sleeping on chan.
// The qlock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes
void wakeup_one(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #11] [4] LOCKS - wakeup_one
		//COMMENT THE FOLLOWING LINE BEFORE START CODING
		//panic("wakeup_one is not implemented yet");
		//Your Code is Here...
	 //  acquire_spinlock to unlock
	 // Acquire the lock for process queues
	if (queue_size(&chan->queue) > 0) {

				 //  acquire_spinlock to unlock
				 acquire_spinlock(&ProcessQueues.qlock);

			        // Dequeue one blocked process from the channel
			        struct Env *blockedbas_env_bta3tna = dequeue(&chan->queue);
			        blockedbas_env_bta3tna ->env_status=ENV_READY;

			        // Insert the process into the ready queue
			         sched_insert_ready(blockedbas_env_bta3tna );

			        // struct Env *kk=blockedbas_env_bta3tna;
		                   release_spinlock(&ProcessQueues.qlock);

			    }

}

//====================================================
// 4) WAKEUP ALL BLOCKED PROCESSES ON A GIVEN CHANNEL:
//====================================================
// Wake up all processes sleeping on chan.
// The queues lock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes
void wakeup_all(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #12] [4] LOCKS - wakeup_all
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("wakeup_all is not implemented yet");
	//Your Code is Here...

	 acquire_spinlock(&ProcessQueues.qlock);

	    while (queue_size(&chan->queue) > 0) {
	        // hnDequeue al blocked process mn al channel
	        struct Env *blockedomnia_env = dequeue(&chan->queue);
	        blockedomnia_env->env_status=ENV_READY;
	        // hnInsert the process fe al ready queue
	        sched_insert_ready(blockedomnia_env);

	    }


	    // Release the process queue lock
	    release_spinlock(&ProcessQueues.qlock);


}

