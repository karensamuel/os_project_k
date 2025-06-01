#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	if (numOfFrames <= 0) {
			// cprintf("Invalid number of frames");
		        return NULL;
		    }
	struct FrameInfo** max_arr = (struct FrameInfo**) kmalloc(numOfFrames * sizeof(struct FrameInfo*));


		    if (max_arr == NULL) {
		   // 	cprintf("Mafeesh makan fel kernel heap");
		        return NULL;
		    }

		    for (int i = 0; i < numOfFrames; i++) {
		        max_arr[i] = 0;
		    }
		    return max_arr;

}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_share is not implemented yet");
	//Your Code is Here...
	 struct Share* create_shares = (struct Share*) kmalloc(sizeof(struct Share));
	 if (create_shares == NULL) {
	         return NULL;
	     }
		void*  va = create_shares;


	         create_shares->ownerID=ownerID;
	         create_shares->size=size;
			 create_shares->isWritable=isWritable;

			 strcpy(create_shares->name, shareName);
			 create_shares->references=1;
			 create_shares->ID=(int32)va & 0x7FFFFFFF;
			 int num_of_frames= ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;

	         create_shares->framesStorage  = create_frames_storage(num_of_frames);
	         	if(create_shares->framesStorage== NULL)
	         	{

	         		kfree(create_shares);
	         		return NULL;
	         	}

         return create_shares;

}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
   //panic("get_share is not implemented yet");
	//Your Code is Here...

	struct Share* share;


	LIST_FOREACH(share, &AllShares.shares_list) {
	   if( ownerID == share->ownerID && strcmp(name, share->name)==0)
	   {

	   return share;
	   }

	}


	return NULL;

}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("createSharedObject is not implemented yet");
	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment
	 acquire_spinlock(&AllShares.shareslock);

	 if ((uint32)virtual_address % PAGE_SIZE != 0) {
		 release_spinlock(&AllShares.shareslock);
	        return E_NO_SHARE;
	    }
	 struct Share* iscreated=get_share(ownerID,shareName);
	 	if(iscreated!=NULL){
	 		 release_spinlock(&AllShares.shareslock);
	 		return E_SHARED_MEM_EXISTS;
	 	}
	struct Share* new_share=create_share(ownerID,shareName,size,isWritable);
	if(new_share== NULL){
		 release_spinlock(&AllShares.shareslock);
		return E_NO_SHARE ;
	}

	int num_of_frames = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;

	  new_share->framesStorage = (struct FrameInfo**)kmalloc(num_of_frames * sizeof(struct FrameInfo*));
	if (new_share->framesStorage == NULL) {
		LIST_REMOVE(&AllShares.shares_list, new_share);
	    kfree(new_share);
	    release_spinlock(&AllShares.shareslock);
	    return E_NO_SHARE;
	}



    LIST_INSERT_HEAD(&AllShares.shares_list, new_share);



    for(int i=0;i<num_of_frames;i++){
    void * va=(void*)((uint32)virtual_address + (i * PAGE_SIZE));
    	struct FrameInfo* frame = NULL;
   int success_alloc= allocate_frame(&frame);

   if(success_alloc!= 0||frame == NULL){

   for (int j = 0; j < i; j++) {
       if (new_share->framesStorage[j]) {
           free_frame(new_share->framesStorage[j]);
           new_share->framesStorage[j] = NULL;
       }
   }
       kfree(new_share->framesStorage);

               LIST_REMOVE(&AllShares.shares_list, new_share);

             kfree(new_share);
             release_spinlock(&AllShares.shareslock);
   return E_NO_SHARE ;
    }
   int perm = PERM_USED|PERM_USER|PERM_PRESENT|PERM_WRITEABLE;


   int success_map= map_frame((uint32*)myenv->env_page_directory,frame,(uint32)va,perm);
	if(success_map!=0){


	for (int j = 0; j < i; j++) {
	    if (new_share->framesStorage[j]) {
	        free_frame(new_share->framesStorage[j]);
	        new_share->framesStorage[j] = NULL;
	    }
	    void* delet_va = (void*)((uint32)virtual_address + (j * PAGE_SIZE));
	    unmap_frame((void*)myenv->env_page_directory, (uint32)delet_va);
	}
		 unmap_frame(myenv->env_page_directory, (uint32)va);


		            LIST_REMOVE(&AllShares.shares_list, new_share);

		            for (int j = 0; j < num_of_frames; j++) {
		                if (new_share->framesStorage[j]) {
		                    free_frame(new_share->framesStorage[j]);
		                }
		            }
	               kfree(new_share->framesStorage);
		            kfree(new_share);
		            release_spinlock(&AllShares.shareslock);
		 return E_NO_SHARE ;
	}

	 new_share->framesStorage[i] = frame;

}

    release_spinlock(&AllShares.shareslock);
  return new_share->ID;

}


//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
//	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
//	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	//panic("getSharedObject is not implemented yet");
//	//Your Code is Here...
//
	struct Env* myenv = get_cpu_proc();

	acquire_spinlock(&AllShares.shareslock);

	if ((uint32)virtual_address % PAGE_SIZE != 0) {
		 release_spinlock(&AllShares.shareslock);
			        return E_NO_SHARE;
			    }
			 struct Share* share_object=get_share(ownerID,shareName);
			 	if(share_object==NULL){
			 		 release_spinlock(&AllShares.shareslock);
			 		return 	E_SHARED_MEM_NOT_EXISTS;
			 	}
		int num_of_frames = ROUNDUP(share_object->size, PAGE_SIZE) / PAGE_SIZE;
		struct FrameInfo* frame =NULL;
		int perm = PERM_PRESENT | PERM_USED|PERM_USER;
		       if (share_object->isWritable) {
		           perm |= PERM_WRITEABLE;
		       }
		for (int i=0;i<num_of_frames;i++){
			void * va=(void*)((uint32)virtual_address + (i * PAGE_SIZE));
			 frame =share_object->framesStorage[i];
			 int success_map= map_frame((void*)myenv->env_page_directory,frame,(uint32)va,perm);


			 if(success_map!=0){

				 release_spinlock(&AllShares.shareslock);
			 	return E_NO_SHARE ;
			 }
		}

	   share_object->references++;
	   release_spinlock(&AllShares.shareslock);
	    return share_object->ID;

}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_share is not implemented yet");
	//Your Code is Here...
	 if (ptrShare == NULL) {
	        return;
	    }
	 //acquire_spinlock(&AllShares.shareslock);
	 	 LIST_REMOVE(&AllShares.shares_list, ptrShare);
	 	 //release_spinlock(&AllShares.shareslock);
	 if (ptrShare->framesStorage != NULL) {

	int num_of_frames = ROUNDUP(ptrShare->size, PAGE_SIZE) / PAGE_SIZE;
	 //cprintf("num of frames to be freed :%d",num_of_frames);
	 for (int j = 0; j < num_of_frames; j++) {
	       if (ptrShare->framesStorage[j]) {
	           free_frame(ptrShare->framesStorage[j]);
	           ptrShare->framesStorage[j] = NULL;
	       }
	   }
	   kfree(ptrShare->framesStorage);
	 }
	               kfree(ptrShare);
 // cprintf("Successfully freed shared object\n");

}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("freeSharedObject is not implemented yet");
	//Your Code is Here...
	   struct Env *myenv = get_cpu_proc();
	    struct Share *share, *share_found = NULL;

	    // Acquire spinlock for thread safety
	    acquire_spinlock(&AllShares.shareslock);

	    // Find the shared object by ID
	    LIST_FOREACH(share, &AllShares.shares_list) {
	        if (share->ID == sharedObjectID) {
	            share_found = share;
	            break;
	        }
	    }

	    // If not found, release the spinlock and return
	    if (share_found == NULL) {
	        release_spinlock(&AllShares.shareslock);
	        return 0;
	    }

	    // Get number of frames to free
	    int num_of_frames = ROUNDUP(share_found->size, PAGE_SIZE) / PAGE_SIZE;

	    // Free each frame
	    for (int i = 0; i < num_of_frames; i++) {
	        void *delete_va = (void *)((uint32)startVA + (i * PAGE_SIZE));
	        unmap_frame((void *)myenv->env_page_directory, (uint32)delete_va);
	        share_found->framesStorage[i] = NULL;
	    }

	    // Check and clear the page table if empty
	    uint32 *ptr_page_table = NULL;
	    get_page_table((void *)myenv->env_page_directory, (uint32)startVA, &ptr_page_table);

	    if (ptr_page_table != NULL) {
	        int is_empty = 1;
	        for (int i = 0; i < 1024; i++) {
	            if (ptr_page_table[i] & PERM_PRESENT) {
	                is_empty = 0;
	                break;
	            }
	        }

	        if (is_empty) {
	            uint32 directory_entry = myenv->env_page_directory[PDX(startVA)];
	            uint32 page_table_physical_address = directory_entry & 0xFFFFF000;
	            pd_clear_page_dir_entry((void *)myenv->env_page_directory, (uint32)startVA);

	            struct FrameInfo *frame = to_frame_info(page_table_physical_address);
	            if (frame != NULL) {
	                free_frame(frame);
	            }
	        }
	    }

	    // Decrement references and free the object if necessary
	    if (share_found->references > 0) {
	        --(share_found->references);
	    }

	    if (share_found->references == 0) {
	        // Release spinlock before calling free_share (avoiding nested locks)
	        release_spinlock(&AllShares.shareslock);
	        free_share(share_found);
	        return 0;
	    }

	    // Release spinlock
	    release_spinlock(&AllShares.shareslock);

	    // Flush the TLB to ensure changes take effect
	    tlbflush();
	    return 0; // Success

	    }

