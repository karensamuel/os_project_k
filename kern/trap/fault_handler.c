/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <kern/cpu/cpu.h>
#include <kern/disk/pagefile_manager.h>
#include <kern/mem/memory_manager.h>

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;
//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//==================
// [1] MAIN HANDLER:
//==================
/*2022*/
uint32 last_eip = 0;
uint32 before_last_eip = 0;
uint32 last_fault_va = 0;
uint32 before_last_fault_va = 0;
int8 num_repeated_fault  = 0;

struct Env* last_faulted_env = NULL;
void fault_handler(struct Trapframe *tf)
{
	/******************************************************/
	// Read processor's CR2 register to find the faulting address
	uint32 fault_va = rcr2();
	//	cprintf("\n************Faulted VA = %x************\n", fault_va);
	//	print_trapframe(tf);
	/******************************************************/

	//If same fault va for 3 times, then panic
	//UPDATE: 3 FAULTS MUST come from the same environment (or the kernel)
	struct Env* cur_env = get_cpu_proc();

	if (last_fault_va == fault_va && last_faulted_env == cur_env)
	{
		num_repeated_fault++ ;
		if (num_repeated_fault == 3)
		{
			print_trapframe(tf);
			panic("Failed to handle fault! fault @ at va = %x from eip = %x causes va (%x) to be faulted for 3 successive times\n", before_last_fault_va, before_last_eip, fault_va);
		}
	}
	else
	{
		before_last_fault_va = last_fault_va;
		before_last_eip = last_eip;
		num_repeated_fault = 0;
	}
	last_eip = (uint32)tf->tf_eip;
	last_fault_va = fault_va ;
	last_faulted_env = cur_env;
	/******************************************************/
	//2017: Check stack overflow for Kernel
	int userTrap = 0;
	if ((tf->tf_cs & 3) == 3) {
		userTrap = 1;
	}
	if (!userTrap)
	{
		struct cpu* c = mycpu();
		//cprintf("trap from KERNEL\n");
		if (cur_env && fault_va >= (uint32)cur_env->kstack && fault_va < (uint32)cur_env->kstack + PAGE_SIZE)
			panic("User Kernel Stack: overflow exception!");
		else if (fault_va >= (uint32)c->stack && fault_va < (uint32)c->stack + PAGE_SIZE)
			panic("Sched Kernel Stack of CPU #%d: overflow exception!", c - CPUS);
#if USE_KHEAP
		if (fault_va >= KERNEL_HEAP_MAX)
			panic("Kernel: heap overflow exception!");
#endif
	}
	//2017: Check stack underflow for User
	else
	{
		//cprintf("trap from USER\n");
		if (fault_va >= USTACKTOP && fault_va < USER_TOP)
			panic("User: stack underflow exception!");
	}

	//get a pointer to the environment that caused the fault at runtime
	//cprintf("curenv = %x\n", curenv);
	struct Env* faulted_env = cur_env;
	//check the faulted address, is it a table or not ?
	//If the directory entry of the faulted address is NOT PRESENT then
	if ( (faulted_env->env_page_directory[PDX(fault_va)] & PERM_PRESENT) != PERM_PRESENT)
	{
		// we have a table fault =============================================================
		//		cprintf("[%s] user TABLE fault va %08x\n", curenv->prog_name, fault_va);
		//		print_trapframe(tf);

		faulted_env->tableFaultsCounter ++ ;

		table_fault_handler(faulted_env, fault_va);
	}
	else
	{
		if (userTrap)
		{
		//	cprintf("ENTERRRRRRRRRRRRRRRRRRRRRRRRRRR");
			/*============================================================================================*/
			//TODO: [PROJECT'24.MS2 - #08] [2] FAULT HANDLER I - Check for invalid pointers
			//(e.g. pointing to unmarked user heap page, kernel or wrong access rights),
			//your code is here

			uint32 x = pt_get_page_permissions(faulted_env->env_page_directory,fault_va);
	//		cprintf("Permissions for VA 0x%x: 0x%x\n", fault_va, x);
			           if (fault_va<KERNEL_HEAP_MAX&&fault_va>=USER_LIMIT)
			           {
								env_exit();
			           }

			if(fault_va<USER_HEAP_MAX&&fault_va>=USER_HEAP_START){

				//cprintf("Done");
				  if(!(x & PERM_MARKED)){
				//	  cprintf("Done1");
					env_exit();
							}
				}

				if ((x & PERM_PRESENT) && !(x & PERM_WRITEABLE)){
			       // cprintf("Done3");
					env_exit();
							}

			/*============================================================================================*/
		}

		/*2022: Check if fault due to Access Rights */
		int perms = pt_get_page_permissions(faulted_env->env_page_directory, fault_va);
		if (perms & PERM_PRESENT)
			panic("Page @va=%x is exist! page fault due to violation of ACCESS RIGHTS\n", fault_va) ;
		/*============================================================================================*/


		// we have normal page fault =============================================================
		faulted_env->pageFaultsCounter ++ ;

		//		cprintf("[%08s] user PAGE fault va %08x\n", curenv->prog_name, fault_va);
		//		cprintf("\nPage working set BEFORE fault handler...\n");
		//		env_page_ws_print(curenv);

		if(isBufferingEnabled())
		{
			__page_fault_handler_with_buffering(faulted_env, fault_va);
		}
		else
		{
			//page_fault_handler(faulted_env, fault_va);
			page_fault_handler(faulted_env, fault_va);
		}
		//		cprintf("\nPage working set AFTER fault handler...\n");
		//		env_page_ws_print(curenv);


	}

	/*************************************************************/
	//Refresh the TLB cache
	tlbflush();
	/*************************************************************/
}

//=========================
// [2] TABLE FAULT HANDLER:
//=========================
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//=========================
// [3] PAGE FAULT HANDLER:
//=========================
void page_fault_handler(struct Env * faulted_env, uint32 fault_va)
{
#if USE_KHEAP
		struct WorkingSetElement *victimWSElement = NULL;
		uint32 wsSize = LIST_SIZE(&(faulted_env->page_WS_list));
#else
		int iWS =faulted_env->page_last_WS_index;
		uint32 wsSize = env_page_ws_get_size(faulted_env);
#endif

	if(wsSize < (faulted_env->page_WS_max_size))
	{
    	//cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWE\n");
		//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
		//TODO: [PROJECT'24.MS2 - #09] [2] FAULT HANDLER I - Placement
		// Write your code here, remove the panic and write your code
		//refer to the project presentation and documentation for details
		struct FrameInfo * ptr=NULL;
					int checklocated = allocate_frame(&ptr);
					int checkMaping= map_frame(faulted_env->env_page_directory,ptr,fault_va,PERM_USER|PERM_WRITEABLE);
					bool flag1=0;
					int returnedVal=pf_read_env_page(faulted_env,(void*)fault_va);
					if(returnedVal == E_PAGE_NOT_EXIST_IN_PF)
					{
						//cprintf("HAAAAAAAAAAAAAAAAAAAAAAA");
						if((fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX) || (fault_va >= USTACKBOTTOM+PAGE_SIZE && fault_va < USTACKTOP)){
						//	cprintf("A333333333333333333\n");
							flag1=1;
						}
						else
						{

							env_exit();
						}
					}
					if(flag1){
					//	cprintf("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
						struct WorkingSetElement* NewWorkingsetElement=env_page_ws_list_create_element(faulted_env,fault_va);
						LIST_INSERT_TAIL(&faulted_env->page_WS_list,NewWorkingsetElement);
						if(++wsSize == (faulted_env->page_WS_max_size))
						{
							//cprintf("GOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
							faulted_env->page_last_WS_element=LIST_FIRST(&faulted_env->page_WS_list);

						//	 cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );

						}
		}
	}
	else
	{

		//cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );
				//refer to the project presentation and documentation for details
				//TODO: [PROJECT'24.MS3] [2] FAULT HANDLER II - Replacement
				// Write your code here, remove the panic and write your code
			    //panic("page_fault_handler() Replacement is not implemented yet...!!");
		struct WorkingSetElement *victim_page_karen = faulted_env->page_last_WS_element;
		struct FrameInfo *frame_info = NULL;
		uint32 *ptr_page_table = NULL;

		//env_page_ws_print(faulted_env);

		while (1) {
			victim_page_karen = faulted_env->page_last_WS_element;

		    uint32 permissions = pt_get_page_permissions(faulted_env->env_page_directory, victim_page_karen->virtual_address);
		    frame_info = get_frame_info(faulted_env->env_page_directory, victim_page_karen->virtual_address, &ptr_page_table);


		    uint32 max_sweeps;
		    if (page_WS_max_sweeps > 0) {
		        max_sweeps = page_WS_max_sweeps;
		    } else {
		        int k = page_WS_max_sweeps * -1;
		        max_sweeps = (permissions & PERM_MODIFIED) ? k + 1 : k;
		    }

		    if (permissions & PERM_USED) {
		        pt_set_page_permissions(faulted_env->env_page_directory, victim_page_karen->virtual_address, 0, PERM_USED);
		        victim_page_karen->sweeps_counter = 0;
		    } else {

		    	victim_page_karen->sweeps_counter++;

		        if (victim_page_karen->sweeps_counter >= max_sweeps) {

		            if (permissions & PERM_MODIFIED) {
		                pf_update_env_page(faulted_env, victim_page_karen->virtual_address, frame_info);
		            }

		            struct WorkingSetElement *after_vicpg = LIST_NEXT(victim_page_karen);
		            env_page_ws_invalidate(faulted_env, victim_page_karen->virtual_address);

		            struct FrameInfo *new_frame = NULL;
		            allocate_frame(&new_frame);
		            map_frame(faulted_env->env_page_directory, new_frame, fault_va, PERM_WRITEABLE | PERM_USER | PERM_USED);
		            int result = pf_read_env_page(faulted_env, (void *)fault_va);
		            if (result == E_PAGE_NOT_EXIST_IN_PF) {
		                if (!((fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX) || (fault_va >= USTACKBOTTOM && fault_va < USTACKTOP))) {

		                    return;
		                }
		            }

		            struct WorkingSetElement *new_pg_omnia  = env_page_ws_list_create_element(faulted_env, fault_va);
		            new_pg_omnia->sweeps_counter = 0;

		            if (after_vicpg == NULL) {
		                LIST_INSERT_TAIL(&faulted_env->page_WS_list, new_pg_omnia);
		            } else {
		                LIST_INSERT_BEFORE(&faulted_env->page_WS_list, after_vicpg, new_pg_omnia);
		            }

		            faulted_env->page_last_WS_element = LIST_NEXT(new_pg_omnia);
		            if (faulted_env->page_last_WS_element == NULL) {
		                faulted_env->page_last_WS_element = LIST_FIRST(&faulted_env->page_WS_list);
		            }

		            env_page_ws_print(faulted_env);
		            return;
		        }
		    }

		    faulted_env->page_last_WS_element = LIST_NEXT(victim_page_karen);
		    if (faulted_env->page_last_WS_element == NULL) {
		        faulted_env->page_last_WS_element = LIST_FIRST(&faulted_env->page_WS_list);
		    }
		}
		//env_page_ws_print(faulted_env);

     }

	}





void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	//[PROJECT] PAGE FAULT HANDLER WITH BUFFERING
	// your code is here, remove the panic and write your code
	//panic("__page_fault_handler_with_buffering() is not implemented yet...!!");
}

