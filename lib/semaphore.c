// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	// what is the size of semaphore when staring
	//cprintf("...creating..\n");

	struct semaphore sem;
	sem.semdata = (struct __semdata*)smalloc((char *)semaphoreName, sizeof(struct __semdata),1);

	if(sem.semdata==NULL){
			//cprintf("smalloc failes to allocate space sem");
			sem.semdata=NULL;
			return sem;
		}


	sem.semdata->count=value;
	strcpy(sem.semdata->name, semaphoreName);
	sem.semdata->lock=0;
	//cprintf("Semaphore data name: %p\n", sem.semdata->name);
	sys_initqueue(&(sem.semdata->queue));
	 if(&(sem.semdata->queue)==NULL){
	    	 //cprintf("ques == 0\n");
	     }
	 //cprintf("Semaphore data queue: %p\n", &(sem.semdata->queue));
	return sem;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//cprintf("...get_semaphore..\n");
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...
	 struct __semdata* shared_sem =sget(ownerEnvID , semaphoreName);
		 struct semaphore sem;
		 sem.semdata = NULL; // Default error state

		 	if (shared_sem == NULL) {
		 		// Handle case where semaphore does not exist
		 	//	cprintf("sget failed: Semaphore '%s' not found for owner %d\n", semaphoreName, ownerEnvID);
		 		return sem; // Return with semdata = NULL
		 	}
		 	//cprintf("Semaphore data name: %p\n", sem.semdata->name);
		 sem.semdata = shared_sem;
		 return sem;

}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
	//cprintf("...wait_semaphore..\n");

	// cprintf("Semaphore data name: %p\n", sem.semdata->name);
	  //cprintf("Semaphore data address: %p\n", sem.semdata);
	    //cprintf("Lock value before loop: %d\n", sem.semdata->lock);
	    //cprintf("Count value before wait: %d\n", sem.semdata->count);


	 //cprintf("waiting\n");


         while(xchg(&(sem.semdata->lock),1)!=0);
		 //cprintf("waiting2\n");
		// cprintf("count before :%d\n",sem.semdata->count);
		(sem.semdata->count)--;
		//cprintf("count after :%d\n",sem.semdata->count);
		if(sem.semdata->count < 0)
		{
			//cprintf("[DEBUG] Semaphore data queue address: %p\n", sem.semdata->queue);
		  //cprintf("[DEBUG] Lock value: %d, Count: %d\n", sem.semdata->lock, sem.semdata->count);
		 // sys_enqueue(&(sem.semdata->queue),(struct Env*)myEnv);
		 /*sys_acquire_spin();
			sys_enqueue(&(sem.semdata->queue),(struct Env*)myEnv);
			cprintf("***********enquing is done ***************\n");

			myEnv->env_status=ENV_BLOCKED;

			sys_fos_scheduler();
			sys_release_spin();
		  (struct Env_Queue*) sem.semdata->queue;*/
		  sem.semdata->lock=0;
		  sys_sleep_sem(&(sem.semdata->queue));

		}
		sem.semdata->lock=0;
	//	cprintf("... finish wait_semaphore..\n");
}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your (Code is Here...
	//cprintf("...signal_semaphore..\n");
	//cprintf("Semaphore data name: %p\n", sem.semdata->name);



   while(xchg(&(sem.semdata->lock),1)!=0);


	(sem.semdata->count)++;
	if(sem.semdata->count<0||sem.semdata->count==0){
	// remove process from queue of semaphore
		//cprintf("[DEBUG] Semaphore data queue address: %p\n", sem.semdata->queue);
		//cprintf("[DEBUG] Lock value: %d, Count: %d\n", sem.semdata->lock, sem.semdata->count);
		/*sys_acquire_spin();
	struct Env* p =(struct Env*) sys_dequeue(&(sem.semdata->queue));
	sys_schedinsertready(p);
	sys_release_spin();
	if (p==NULL){
		cprintf("queue is empty");
		sem.semdata->lock=0;
		return;
	}*/
		sem.semdata->lock=0;
		sys_sleep_sem_sig(&(sem.semdata->queue));
	//add process to readylist

	//sys_enqueue(&(ProcessQueues->env_ready_queues[p->priority]), p);

	}
	//cprintf("signal\n");
	sem.semdata->lock=0;
	//cprintf("...end_signal_semaphore..\n");
}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
