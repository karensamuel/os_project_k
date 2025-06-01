#include <inc/lib.h>

#define uheap_frames (USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE
uint32 taken_u[uheap_frames]={0} ;
uint32 Num_Of_Pages_to_free[uheap_frames]={0};

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/

void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy
	uint32 *ret= NULL;
	 //cprintf("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");
			if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
				//cprintf("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n");
						if (sys_isUHeapPlacementStrategyFIRSTFIT()) {
							ret = alloc_block_FF((uint32) size);
							return ret;
							  // Return if successfully allocated with first-fit strategy
						} else if (sys_isUHeapPlacementStrategyBESTFIT()) {
							ret = alloc_block_BF((uint32) size);
							return ret;
							  // Return if successfully allocated with best-fit strategy
						}
				 }
			 else
			 {
				 //cprintf("ooooooooooooooooooooooooooooooooooooooo\n");
				 uint32 str_pg_alloc = myEnv->uh_block_hlimit + PAGE_SIZE;
				 uint32 max_itr = ((uint32)USER_HEAP_MAX - str_pg_alloc)/PAGE_SIZE;
				 uint32 neededSize = ROUNDUP(size, PAGE_SIZE);  // Round up to the nearest page size
				 	 int fit_pages = neededSize / PAGE_SIZE;         // Calculate the number of pages required
				 	 int counter = 0;
				 	 uint32 fit_flag = 0 ;
				 	 uint32 returnStart = 0;
				 uint32 itr = 0;
				 uint32 trev = str_pg_alloc;
						while(trev < (uint32)USER_HEAP_MAX && itr<max_itr)
						{
							if(!taken_u[(trev - USER_HEAP_START)/PAGE_SIZE])
							{
								if(!returnStart)
								{
									returnStart = trev;

								}

								counter++;
								trev += PAGE_SIZE;
								itr++;
								if(fit_pages <= counter)
								{
									fit_flag = 1;
									break;
								}

							}
							else
							{
								itr++;
								counter = 0;
								returnStart = 0;
								fit_flag = 0;
								trev += PAGE_SIZE;
							}
						}

						if(fit_flag)
							{
							     uint32 assign = returnStart;
							     for (uint32 i = 0; i < fit_pages; i++)
								 {
									 taken_u[((assign + i * PAGE_SIZE) - USER_HEAP_START) / PAGE_SIZE] = 1;
								 }

							    	int pagenumber=(uint32)(returnStart-USER_HEAP_START)/PAGE_SIZE;
							         Num_Of_Pages_to_free[pagenumber]=fit_pages;
								sys_allocate_user_mem(assign , size);
								ret  = (void *)returnStart;
							}
							else
							{
								ret = NULL;
							}

			 }
			//cprintf("PPPPPPeeeeeeeeeeeeeeeooooooooooooooo\n");
			return ret;


}



//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	     int pages_to_free ;


    if (virtual_address >= (void*)(myEnv->start_uheap) && virtual_address <(void*)(myEnv->uh_block_hlimit))
    {


    	//cprintf("done\n");
    	  free_block(virtual_address);
    		//cprintf("done1\n");

    	return;
    }
    else if(virtual_address >= (void*)(myEnv->uh_block_hlimit+PAGE_SIZE) && virtual_address <(void*)USER_HEAP_MAX)
    {
    	//cprintf("done2\n");

    	int pagenumber=(uint32)(virtual_address-USER_HEAP_START)/PAGE_SIZE;

        int pages_to_free=Num_Of_Pages_to_free[pagenumber];

				 uint32 Size = pages_to_free*PAGE_SIZE;
					//cprintf("done3\n");
				 sys_free_user_mem((uint32)virtual_address,Size);
			         for(int i = 0; i < pages_to_free; ++i) {
						 taken_u[pagenumber+i]=0;
			         }

					 Num_Of_Pages_to_free[pagenumber]=0;
     }
    else
    {

        panic("Invalid address passed to free(could be illegal)");
    }

}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	uint32 *ret= NULL;
		 uint32 str_pg_alloc = myEnv->uh_block_hlimit+ PAGE_SIZE;
		 if (size >USER_HEAP_MAX - str_pg_alloc - PAGE_SIZE){
		 		return NULL;
		 	}
						 uint32 max_itr = (USER_HEAP_MAX - str_pg_alloc)/PAGE_SIZE;
						 uint32 neededSize = ROUNDUP(size, PAGE_SIZE);
						 	 int fit_pages = neededSize / PAGE_SIZE;
						 	if (fit_pages * PAGE_SIZE > neededSize) {
						 	    fit_pages--;
						 	}
						 	// cprintf("needed pages %d: for size :%d \n",fit_pages,neededSize);
						 	 int counter = 0;
						 	 uint32 fit_flag = 0 ;
						 	 uint32 returnStart = 0;
						 uint32 itr = 0;
						 uint32 trev = str_pg_alloc;

						 //cprintf("the value of trev:%d\n",trev);

								while(trev < (uint32)USER_HEAP_MAX && itr < max_itr)
								{


									if(taken_u[(trev - USER_HEAP_START)/PAGE_SIZE]==0)
									{
										if(!returnStart)
										{


											returnStart = trev;

										}

										counter++;
										trev += PAGE_SIZE;
										itr++;
										if(fit_pages <= counter)
										{
											fit_flag = 1;
											break;
										}

									}
									else
									{
										itr++;
										counter = 0;
										returnStart = 0;
										fit_flag = 0;
										trev += PAGE_SIZE;
									}
								}



		if(fit_flag)
				{
				 uint32 assign = returnStart;
				//	cprintf("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

				int created= sys_createSharedObject((char* )sharedVarName,size,isWritable,(void*)returnStart);



if(created ==E_SHARED_MEM_EXISTS|| created ==E_NO_SHARE )
	{
	 for (int i = 0; i < counter; i++) {
	        taken_u[((returnStart - USER_HEAP_START) / PAGE_SIZE) + i] = 0;
	    }
		return NULL;
	}
for (int i = 0 ;  i < fit_pages ;  i++)
{
	//cprintf("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");

taken_u[((assign - USER_HEAP_START) / PAGE_SIZE)] = created;
assign += PAGE_SIZE;

}
				}
		int pagenumber = (returnStart - USER_HEAP_START) / PAGE_SIZE;
		if (pagenumber < 0 || pagenumber >= max_itr) {
		    return NULL; // Ensure the page number is within bounds
		}
		Num_Of_Pages_to_free[pagenumber] = fit_pages;
ret  = (uint32 *)returnStart;


return ret;
	}
//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
// Write your code here, remove the panic and write your code
//panic("sget() is not implemented yet...!!");
// 1)get the size using sys_getsize
void* sget(int32 ownerEnvID, char *sharedVarName)
{

	//cprintf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget(
	int size =sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
			if(size== 0){
	return NULL;
			}

	// 2)apply first fit  search heap for space
			uint32 *ret= NULL;
				 uint32 str_pg_alloc = myEnv->uh_block_hlimit + PAGE_SIZE;
								 uint32 max_itr = (USER_HEAP_MAX - str_pg_alloc)/PAGE_SIZE;
								 uint32 neededSize = ROUNDUP(size, PAGE_SIZE);
								 	 int fit_pages = neededSize / PAGE_SIZE;
								 	 int counter = 0;
								 	 uint32 fit_flag = 0 ;
								 	 uint32 returnStart = 0;
								 uint32 itr = 0;
								 uint32 trev = str_pg_alloc;
				while(trev < (uint32)USER_HEAP_MAX && itr < max_itr)
					{
						if(taken_u[(trev - USER_HEAP_START)/PAGE_SIZE]==0)
							{
								if(!returnStart)
							             {
								returnStart = trev;

							    }

							counter++;
							trev += PAGE_SIZE;
							itr++;
							if(fit_pages <= counter)
							{
								fit_flag = 1;
								break;
							}

						  }
								else
									{
											itr++;
											counter = 0;
	                                           returnStart = 0;
												fit_flag = 0;
												trev += PAGE_SIZE;
											}
										}


	if(fit_flag)
	{

		 uint32 assign = returnStart;

		//	cprintf("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
		 int alloc=sys_getSharedObject(ownerEnvID,sharedVarName,(void*)assign);

		//	cprintf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY");
		 	if(alloc==E_SHARED_MEM_NOT_EXISTS){
		 		return NULL;
		 	}
	for (int i = 0 ;  i < fit_pages ;  i++)

	{
		//cprintf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
	taken_u[((assign + i * PAGE_SIZE) - USER_HEAP_START) / PAGE_SIZE] = 1;
	assign += PAGE_SIZE;

	}
             	ret  = (void *)returnStart;
          	return ret;
	   }
	else
	{
     	return NULL;
	}


}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
		// Write your code here, remove the panic and write your code
		//panic("sfree() is not implemented yet...!!");
    // Validate virtual address
    if (virtual_address == NULL || (uint32)virtual_address < USER_HEAP_START ) {
       // cprintf("Error: Invalid virtual address\n");
        return;
    }

    // Calculate index and validate
    uint32 index = (uint32)(virtual_address-USER_HEAP_START)/PAGE_SIZE;


    // Get the shared object ID
    uint32 id = taken_u[index];
    //cprintf("id in free:%d:\n",id);
    if (id == 0) {
        //cprintf("Error: No shared object found for this address\n");
        return;
    }

    // Free the shared object
    int result = sys_freeSharedObject(id, virtual_address);
    if (result != 0) {
        //cprintf("Error: sys_freeSharedObject failed\n");
        return;
    }

    // Get the number of pages to free
    uint32 pagenumber = index;
    int pages_to_free = Num_Of_Pages_to_free[pagenumber];
    if (pages_to_free <= 0) {
       // cprintf("Error: Invalid number of pages to free\n");
        return;
    }

    // Clear the `taken_u` array
    for (int i = 0; i < pages_to_free; i++) {

    	  uint32 current_index = index + i;
    	        taken_u[current_index] = 0;
    }

    // Reset the page count in `Num_Of_Pages_to_free`
    Num_Of_Pages_to_free[pagenumber] = 0;

   // cprintf("sfree: Successfully freed shared object\n");
}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	//panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
