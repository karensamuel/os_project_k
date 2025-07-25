// System call stubs.

#include <inc/syscall.h>
#include <inc/lib.h>

static inline uint32
syscall(int num, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
	uint32 ret;

	// Generic system call: pass system call number in AX,
	// up to five parameters in DX, CX, BX, DI, SI.
	// Interrupt kernel with T_SYSCALL.
	//
	// The "volatile" tells the assembler not to optimize
	// this instruction away just because we don't use the
	// return value.
	//
	// The last clause tells the assembler that this can
	// potentially change the condition codes and arbitrary
	// memory locations.

	asm volatile("int %1\n"
		: "=a" (ret)
		: "i" (T_SYSCALL),
		  "a" (num),
		  "d" (a1),
		  "c" (a2),
		  "b" (a3),
		  "D" (a4),
		  "S" (a5)
		: "cc", "memory");

	return ret;
}

void
sys_cputs(const char *s, uint32 len, uint8 printProgName)
{
	syscall(SYS_cputs, (uint32) s, len, (uint32)printProgName, 0, 0);
}

int
sys_cgetc(void)
{
	return syscall(SYS_cgetc, 0, 0, 0, 0, 0);
}

void sys_lock_cons(void)
{
	syscall(SYS_lock_cons, 0, 0, 0, 0, 0);
}
void sys_unlock_cons(void)
{
	syscall(SYS_unlock_cons, 0, 0, 0, 0, 0);
}

int __sys_allocate_page(void *va, int perm)
{
	return syscall(SYS_allocate_page, (uint32) va, perm, 0 , 0, 0);
}

int __sys_map_frame(int32 srcenv, void *srcva, int32 dstenv, void *dstva, int perm)
{
	return syscall(SYS_map_frame, srcenv, (uint32) srcva, dstenv, (uint32) dstva, perm);
}

int __sys_unmap_frame(int32 envid, void *va)
{
	return syscall(SYS_unmap_frame, envid, (uint32) va, 0, 0, 0);
}

uint32 sys_calculate_required_frames(uint32 start_virtual_address, uint32 size)
{
	return syscall(SYS_calc_req_frames, start_virtual_address, (uint32) size, 0, 0, 0);
}

uint32 sys_calculate_free_frames()
{
	return syscall(SYS_calc_free_frames, 0, 0, 0, 0, 0);
}
uint32 sys_calculate_modified_frames()
{
	return syscall(SYS_calc_modified_frames, 0, 0, 0, 0, 0);
}

uint32 sys_calculate_notmod_frames()
{
	return syscall(SYS_calc_notmod_frames, 0, 0, 0, 0, 0);
}

int sys_pf_calculate_allocated_pages()
{
	return syscall(SYS_pf_calc_allocated_pages, 0,0,0,0,0);
}

int sys_calculate_pages_tobe_removed_ready_exit(uint32 WS_or_MEMORY_flag)
{
	return syscall(SYS_calculate_pages_tobe_removed_ready_exit, WS_or_MEMORY_flag,0,0,0,0);
}

void sys_scarce_memory()
{
	syscall(SYS_scarce_memory,0,0,0,0,0);
}

void
sys_cputc(const char c)
{
	syscall(SYS_cputc, (uint32) c, 0, 0, 0, 0);
}


//NEW'12: BONUS2 Testing
void
sys_clear_ffl()
{
	syscall(SYS_clearFFL,0, 0, 0, 0, 0);
}

int sys_createSharedObject(char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	return syscall(SYS_create_shared_object,(uint32)shareName, (uint32)size, isWritable, (uint32)virtual_address,  0);
}

//2017:
int sys_getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	return syscall(SYS_get_size_of_shared_object,(uint32) ownerID, (uint32)shareName, 0, 0, 0);
}
//==========

int sys_getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	return syscall(SYS_get_shared_object,(uint32) ownerID, (uint32)shareName, (uint32)virtual_address, 0, 0);
}

int sys_freeSharedObject(int32 sharedObjectID, void *startVA)
{
	return syscall(SYS_free_shared_object,(uint32) sharedObjectID, (uint32) startVA, 0, 0, 0);
}

int sys_create_env(char* programName, unsigned int page_WS_size,unsigned int LRU_second_list_size,unsigned int percent_WS_pages_to_remove)
{
	return syscall(SYS_create_env,(uint32)programName, (uint32)page_WS_size,(uint32)LRU_second_list_size, (uint32)percent_WS_pages_to_remove, 0);
}

void sys_run_env(int32 envId)
{
	syscall(SYS_run_env, (int32)envId, 0, 0, 0, 0);
}

int sys_destroy_env(int32  envid)
{
	return syscall(SYS_destroy_env, envid, 0, 0, 0, 0);
}

int32 sys_getenvid(void)
{
	 return syscall(SYS_getenvid, 0, 0, 0, 0, 0);
}

//2017
int32 sys_getenvindex(void)
{
	 return syscall(SYS_getenvindex, 0, 0, 0, 0, 0);
}

int32 sys_getparentenvid(void)
{
	 return syscall(SYS_getparentenvid, 0, 0, 0, 0, 0);
}


void sys_exit_env(void)
{
	syscall(SYS_exit_env, 0, 0, 0, 0, 0);
}


struct uint64 sys_get_virtual_time()
{
	struct uint64 result;
	syscall(SYS_get_virtual_time, (uint32)&(result.low), (uint32)&(result.hi), 0, 0, 0);
	return result;
}

// 2014
void sys_move_user_mem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	syscall(SYS_move_user_mem, src_virtual_address, dst_virtual_address, size, 0, 0);
	return ;
}
uint32 sys_rcr2()
{
	return syscall(SYS_rcr2, 0, 0, 0, 0, 0);
}

void sys_bypassPageFault(uint8 instrLength)
{
	syscall(SYS_bypassPageFault, instrLength, 0, 0, 0, 0);
	return ;
}
void rsttst()
{
	syscall(SYS_rsttst, 0, 0, 0, 0, 0);
	return ;
}
void tst(uint32 n, uint32 v1, uint32 v2, char c, int inv)
{
	syscall(SYS_testNum, n, v1, v2, c, inv);
	return ;
}
void chktst(uint32 n)
{
	syscall(SYS_chktst, n, 0, 0, 0, 0);
	return ;
}

void inctst()
{
	syscall(SYS_inctst, 0, 0, 0, 0, 0);
	return ;
}
uint32 gettst()
{
	return syscall(SYS_gettst, 0, 0, 0, 0, 0);
}


//2015
uint32 sys_isUHeapPlacementStrategyFIRSTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_FIRSTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyBESTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_BESTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyNEXTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_NEXTFIT)
		return 1;
	else
		return 0;
}
uint32 sys_isUHeapPlacementStrategyWORSTFIT()
{
	uint32 ret = syscall(SYS_get_heap_strategy, 0, 0, 0, 0, 0);
	if (ret == UHP_PLACE_WORSTFIT)
		return 1;
	else
		return 0;
}

void sys_set_uheap_strategy(uint32 heapStrategy)
{
	syscall(SYS_set_heap_strategy, heapStrategy, 0, 0, 0, 0);
	return ;
}

//2020
int sys_check_LRU_lists(uint32* active_list_content, uint32* second_list_content, int actual_active_list_size, int actual_second_list_size)
{
	return syscall(SYS_check_LRU_lists, (uint32)active_list_content, (uint32)second_list_content, (uint32)actual_active_list_size, (uint32)actual_second_list_size, 0);
}

int sys_check_LRU_lists_free(uint32* list_content, int list_size)
{
	return syscall(SYS_check_LRU_lists_free, (uint32)list_content, (uint32)list_size , 0, 0, 0);
}

int sys_check_WS_list(uint32* WS_list_content, int actual_WS_list_size, uint32 last_WS_element_content, bool chk_in_order)
{
	return syscall(SYS_check_WS_list, (uint32)WS_list_content, (uint32)actual_WS_list_size , last_WS_element_content, (uint32)chk_in_order, 0);
}
void sys_allocate_chunk(uint32 virtual_address, uint32 size, uint32 perms)
{
	syscall(SYS_allocate_chunk_in_mem, virtual_address, size, perms, 0, 0);
	return ;
}
void sys_utilities(char* utilityName, int value)
{
	syscall(SYS_utilities, (uint32)utilityName, value, 0, 0, 0);
	return;
}


//TODO: [PROJECT'24.MS1 - #02] [2] SYSTEM CALLS - Implement these system calls
void* sys_sbrk(int increment)
{
	//Comment the following line before start coding...
	//panic("not implemented yet");k
	int kkk =increment;
	 return (void *) syscall(SYS_sbrk, kkk,0,0,0,0);

}

void sys_free_user_mem(uint32 virtual_address, uint32 size)
{
	//Comment the following line before start coding...
	//panic("not implemented yet");
	uint32 tasn= virtual_address;
	 uint32 bas= size;
	syscall(SYS_free_user_mem,tasn,bas,0,0,0);
}

void sys_allocate_user_mem(uint32 virtual_address, uint32 size)
{
	//Comment the following line before start coding...
	//panic("not implemented yet");
	uint32 tasn= virtual_address;
		 uint32 bas= size;

	syscall(SYS_allocate_user_mem,tasn,bas,0,0,0);

}
void sys_initqueue(struct Env_Queue* queue)
{
	syscall(SYS_initqueue, (uint32)queue,0, 0, 0, 0);
	return;
}
void sys_enqueue(struct Env_Queue* queue, struct Env* env)
{
	syscall(SYS_enqueue, (uint32)queue,(uint32)env, 0, 0, 0);
	return;
}
void* sys_dequeue(struct Env_Queue* queue)
{
	return (void *) syscall(SYS_dequeue, (uint32)queue,0, 0, 0, 0);

}
void sys_schedinsertready(struct Env* env)
{
	  syscall(SYS_schedinsertready, (uint32)env,0, 0, 0, 0);
	  return;
}
void sys_fos_scheduler()
{
	  syscall(SYS_fos_scheduler, 0,0, 0, 0, 0);
	  return;
}
void sys_sleep_sem(struct Env_Queue* queue)
{
	syscall(SYS_sleep_sem, (uint32)queue,0, 0, 0, 0);
	return;

}
void sys_sleep_sem_sig(struct Env_Queue* queue)
{
	syscall(SYS_sleep_sem_sig, (uint32)queue,0, 0, 0, 0);
	return;

}
void sys_acquire_spin()
{
	  syscall(SYS_acquire_spin, 0,0, 0, 0, 0);
	  return;
}
void sys_release_spin()
{
	  syscall(SYS_release_spin, 0,0, 0, 0, 0);
	  return;
}
void sys_env_set_priority(int32 envID, int priority)
{

	syscall(SYS_env_set_priority, envID, priority, 0, 0, 0);
}
