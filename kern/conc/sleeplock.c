// Sleeping locks

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "sleeplock.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_sleeplock(struct sleeplock *lk, char *name)
{
	init_channel(&(lk->chan), "sleep lock channel");
	init_spinlock(&(lk->lk), "lock of sleep lock");
	strcpy(lk->name, name);
	lk->locked = 0;
	lk->pid = 0;
}
int holding_sleeplock(struct sleeplock *lk)
{
	int r;
	acquire_spinlock(&(lk->lk));
	r = lk->locked && (lk->pid == get_cpu_proc()->env_id);
	release_spinlock(&(lk->lk));
	return r;
}
//==========================================================================

void acquire_sleeplock(struct sleeplock *lk)
{
	//TODO: [PROJECT'24.MS1 - #13] [4] LOCKS - acquire_sleeplock
		//COMMENT THE FOLLOWING LINE BEFORE START CODING
	   //panic("acquire_sleeplock is not implemented yet");

		 acquire_spinlock(&(lk->lk));

		    // bnstna l8it ma alock yb2a mota7
		    while (lk->locked) {

		        sleep(&(lk->chan), &(lk->lk)); // Release the spinlock while sleeping
		    }
            int kar =1;
		    // Lock al sleeplock
		    lk->locked =  kar;

		    // bnRelease the spinlock b3d ma bn acquire al sleep lock
		    release_spinlock(&(lk->lk));

}

void release_sleeplock(struct sleeplock *lk)
{
	//TODO: [PROJECT'24.MS1 - #14] [4] LOCKS - release_sleeplock
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("release_sleeplock is not implemented yet");
	//Your Code is Here...
	acquire_spinlock(&(lk->lk));
		if(queue_size( &lk->chan.queue)>0){
			wakeup_all(&(lk->chan));
		}


		int mennakk =0;
		lk->locked = mennakk;
		/*int kar =-1;
		 lk->pid = kar;*/

		 release_spinlock(&lk->lk);

}





