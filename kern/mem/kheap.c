#include "kheap.h"
#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"



#define heap_frames (KERNEL_HEAP_MAX - KERNEL_HEAP_START)/ PAGE_SIZE

uint32 Num_OF_Pages[heap_frames] = {0};

int taken[heap_frames]={0} ;


// Function to get the number of pages at a specific address
int alloc_map(uint32 virtualAddress)
{

    struct FrameInfo *frInfo;

    if (allocate_frame(&frInfo) != E_NO_MEM) {

        frInfo->bufferedVA= virtualAddress;


        map_frame(ptr_page_directory, frInfo, virtualAddress, PERM_WRITEABLE | PERM_PRESENT | PERM_MODIFIED);

        return 0;
    }


        return E_NO_MEM;
}

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
		// Write your code here, remove the panic and write your code
	//	panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!"
		initSizeToAllocate = ROUNDUP(initSizeToAllocate, PAGE_SIZE);
		  kheap_start = daStart;
		  kheap_hard_limit = daLimit;
		 brk = (uint32*) (initSizeToAllocate + daStart);


	    uint32 sumbas = initSizeToAllocate + kheap_start;
	    if (sumbas >  kheap_hard_limit) {

	    	 panic("Kernel heap initialization failed: initial size exceeds limit");
	    }

	    uint32 required_size = initSizeToAllocate ;
	    uint32 rara_pages = ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE;


	    if (rara_pages > LIST_SIZE(&MemFrameLists.free_frame_list)) {
	   	    	panic("Kernel heap initialization failed: insufficient memory");
	    }
	    uint32 totalkll = kheap_start + initSizeToAllocate;
	    for (uint32 i = kheap_start; i < totalkll; i += PAGE_SIZE) {


	        if (alloc_map(i) != 0)
	        {
	       	 panic("Kernel heap initialization failed: page allocation error");
	        }
	        taken[(i-KERNEL_HEAP_START)/ PAGE_SIZE]=1;
	    }
	    uint32 index = (kheap_start - KERNEL_HEAP_START) / PAGE_SIZE;
	   	 Num_OF_Pages[index]=((kheap_start+initSizeToAllocate) - KERNEL_HEAP_START) / PAGE_SIZE;

	    initialize_dynamic_allocator(kheap_start, initSizeToAllocate);

	    return 0; // Success
}

void* sbrk(int numOfPages)
{
	/* Num_OF_Pages > 0: move the segment break of the kernel to increase the size of its heap by the given Num_OF_Pages,
		 * 				you should allocate pages and map them into the kernel virtual address space,
		 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
		 * Num_OF_Pages = 0: just return the current position of the segment break
		 *
		 * NOTES:
		 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
		 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
		 */

		//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//	return (void*)-1 ;
		//====================================================

		//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
		// Write your code here, remove the panic and write your code
	//	panic("sbrk() is not implemented yet...!!");


		void* returned_val;
		if(numOfPages == 0)
		{

			return (void*)brk;
		}
		else if(numOfPages > 0)
		{
	//	    the space want to allocate it.
			uint32 space = (uint32)numOfPages * PAGE_SIZE;
			//cast type for old brk
			uint32 brk_add = (uint32)brk;
			//new brk address.
			if(brk_add + space> kheap_hard_limit)
			{
				returned_val=(void*)-1 ;
			}
			else
			{

				brk_add += (space);


				uint32* old_brk = brk;// to be returned
				for(int i = 0; i<numOfPages; i++) {
					alloc_map((uint32)old_brk + i * PAGE_SIZE);
					taken[((uint32)old_brk+i-KERNEL_HEAP_START)/ PAGE_SIZE]=1;
				}
				uint32 index = ((uint32)old_brk - KERNEL_HEAP_START) / PAGE_SIZE;
			   Num_OF_Pages[index]=numOfPages;


				brk = (uint32*)brk_add;
				returned_val =(void*)old_brk;
			}
		}

		return returned_val;

				}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{



	   uint32 neededSize = ROUNDUP(size, PAGE_SIZE);
	    int fit_pages = neededSize / PAGE_SIZE;
	    str_pg_alloc = kheap_hard_limit + PAGE_SIZE;
	    uint32 returnStart = 0;
	    uint32 *ret = NULL;
	    int max_iterations = (KERNEL_HEAP_MAX - str_pg_alloc) / PAGE_SIZE;  // Set max loop iterations

	//    cprintf("Requested kmalloc of size %u, rounded to %u pages.\n", size, fit_pages);

	    if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
	        if (isKHeapPlacementStrategyFIRSTFIT()) {
	            ret = alloc_block_FF((uint32)size);
	   //         cprintf("First fit allocation returned %p\n", ret);


	            return ret;
	        } else if (isKHeapPlacementStrategyBESTFIT()) {
	            ret = alloc_block_BF((uint32)size);
	  //          cprintf("Best fit allocation returned %p\n", ret);

	            return ret;
	        }
	    }

	    uint32 trev = str_pg_alloc;
	    int counter = 0;
	    uint32 fit_flag = 0;
	    int framesAllocated = 0;

	    // Improved loop for finding free space using first-fit strategy
	    for (int i = 0; i < max_iterations && trev < KERNEL_HEAP_MAX; ) {
	        if (!taken[(trev - KERNEL_HEAP_START) / PAGE_SIZE]) {
	            // Found a free page, start counting free contiguous pages
	            if (counter == 0) {
	                returnStart = trev;  // Start of free region
	            }
	            counter++;
	            trev += PAGE_SIZE;
	            i++;  // Increment iteration counter

	            if (counter >= fit_pages) {
	                fit_flag = 1;
	                break;
	            }
	        } else {
	            // Reset and move to the next block after a failed attempt
	            counter = 0;
	            returnStart = 0;
	            trev += PAGE_SIZE;
	            i++;
	        }
	    }

	    // Check if we found a suitable free block
	    if (fit_flag && LIST_SIZE(&MemFrameLists.free_frame_list) >= fit_pages) {
	        uint32 assign = returnStart;
	        for (int i = 0; i < fit_pages; i++)
	        {
	            struct FrameInfo* newFrame = NULL;
	            int result = allocate_frame(&newFrame);
	            if (result == E_NO_MEM)
	            {
	     //           cprintf("Allocation failed: no memory.\n");

	                return NULL;
	            } else
	            {
	                map_frame((void*)ptr_page_directory, newFrame, assign, PERM_WRITEABLE | PERM_PRESENT);
	          //  cprintf("Mapped frame at %p with permissions PERM_WRITEABLE | PERM_PRESENT.\n", (void*)assign);
	                taken[(assign - KERNEL_HEAP_START) / PAGE_SIZE] = 1;
	                newFrame->bufferedVA=assign;
	                assign += PAGE_SIZE;
	                framesAllocated++;
	            }
	        }
	        ret = (void*)returnStart;
	        // Calculate the index based on the address range allocated
	                uint32 index = (returnStart - KERNEL_HEAP_START) / PAGE_SIZE;
	                // Store the total frames allocated at this address index
	                Num_OF_Pages[index] = framesAllocated;  // Increment the total allocated frames in this index

	     //   cprintf("kmalloc successful: returning address %p.\n", ret);
	    } else {
	     //   cprintf("kmalloc failed: insufficient space or frames.\n");
	        ret = NULL;
	    }
	    return ret;

}



void kfree(void* virtual_address)
{


	uint32 addr=(uint32)virtual_address;
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for detail

		    if (virtual_address >= (void*)kheap_start && virtual_address <(void*) kheap_hard_limit) {
		    	//cprintf("done1\n");
		    	free_block(virtual_address);

		    	return;

		    }
	 else if ((virtual_address >= (void*)(PAGE_SIZE+kheap_hard_limit) )&& virtual_address < (void*)KERNEL_HEAP_MAX)
		        {
		        //	cprintf("done3\n");
		          int Num_of_frames=Num_OF_Pages[((uint32)virtual_address-KERNEL_HEAP_START)/PAGE_SIZE];
		    	    // Find the physical address mapped to this virtual address
		    	 uint32 phaddr = kheap_physical_address((uint32)virtual_address);

		   	    if (phaddr == 0) {
		    	        // If no physical address is mapped, nothing to free
		    	        panic("Trying to free an unmapped or invalid address");

		    	        return;
		    	     //   cprintf("done4\n");
		    	    }

	      for(int i=0;i<Num_of_frames;i++)
	      {
	    //	  cprintf("done5\n");

	        	   uint32 *ptr_page_table=NULL;
	        	  struct FrameInfo* ptr_frame_info = get_frame_info(ptr_page_directory, addr, &ptr_page_table);
	        	//  cprintf("done6\n");
	        	  if (ptr_frame_info!=NULL) {
	        		//  cprintf("done7\n");
	        		  ptr_frame_info->bufferedVA=0;
					   unmap_frame(ptr_page_directory,addr);
					  // cprintf("done8\n");
	                      taken[(addr - KERNEL_HEAP_START) / PAGE_SIZE] = 0;
	                      addr += PAGE_SIZE;
	                 }
	      }
	                    Num_OF_Pages[((uint32)virtual_address - str_pg_alloc)/ PAGE_SIZE] = 0;
		        }
	}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================


	if(((void*)virtual_address == NULL))
	{

		return 0;
	}
	uint32 *ptr_page_table_of_vir_add ;
	get_page_table(ptr_page_directory, virtual_address, &ptr_page_table_of_vir_add);

	if (ptr_page_table_of_vir_add!= NULL){
		 int j = PTX(virtual_address);
		if( (ptr_page_table_of_vir_add[j] & PERM_PRESENT) != 0)
		{

	 return (ptr_page_table_of_vir_add[j] & 0xFFFFF000) + (virtual_address & 0x00000FFF);
	}
	}


		return 0;
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");



	if(physical_address == 0)
	{

			return 0;
	}

	uint32 offset =physical_address& 0x00000fff;
		struct FrameInfo* ptr_frame_info = NULL;
		ptr_frame_info = to_frame_info((uint32)physical_address);
		if( ptr_frame_info !=NULL && ptr_frame_info->references>0 ){
			if (ptr_frame_info->bufferedVA != 0){
			uint32 va1=ptr_frame_info->bufferedVA;
			unsigned int va2= (unsigned int)((va1& 0xfffff000 )+offset);


			return va2;
			}
		}

     return 0;

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	//panic("krealloc() is not implemented yet...!!");

	return NULL;
}
