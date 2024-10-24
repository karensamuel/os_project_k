/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================
	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");

	//begin block of heap
	uint32 *Begin_Block , *End_Block;
	//create Begin Block
	Begin_Block = (uint32 *)daStart;
	*Begin_Block = 1;

	//create End Block

	uint32 Heap_Limit = daStart + initSizeOfAllocatedSpace - 4  ;
	 End_Block = (uint32 *)Heap_Limit;
	*End_Block = 1;

//	uint32 *Header_Ptr = Begin_Block;
//	Header_Ptr += 1; //take space for header and size for Begin.

	uint32 *header =  Begin_Block + 1;
	uint32 *footer =  (uint32 *)(daStart + initSizeOfAllocatedSpace - 8) ;
	*header = initSizeOfAllocatedSpace - 8;
	*footer= initSizeOfAllocatedSpace - 8;
	struct BlockElement *Block= (struct BlockElement*)(header +1);
	LIST_INIT(&freeBlocksList);
	LIST_INSERT_HEAD(&freeBlocksList,Block);// in head of list(first Element)
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//	panic("set_block_data is not implemented yet");
	//ptr point to header

	uint32 * blockHeader = (uint32 *)va - 1;//reach header

	uint32 total_moves = (uint32) va + totalSize ;


	//reach to size of footer.
//	va = (void *)((char *)va + totalSize);
	//ptr point to footer
	uint32 * blockFooter = (uint32 *)total_moves;
	if(isAllocated == 1)
	{
		//block is allocated (lsb = 1).
		totalSize +=  1;
	}
	*blockHeader = totalSize;
	*blockFooter = totalSize;
	//for check only
	cprintf("The Block Size: %d, My Block Free? %d\n", get_block_size(va), is_free_block(va));
}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void insert_block_sorted(struct BlockElement *new_block)
{
    struct BlockElement*current;
    LIST_FOREACH(current, &freeBlocksList) {
        // Assuming each block has an address you can compare
        if (new_block < current)
        {
            // Insert before current block
            LIST_INSERT_BEFORE(&freeBlocksList,current,new_block);
            return;
        }
    }
    // If we reached here, it means new_block is the largest, insert at the tail
    LIST_INSERT_TAIL(&freeBlocksList,new_block);
}
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if(size == 0)
		{
			return NULL;
		}
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================
    //	panic("alloc_block_FF is not implemented yet");
    size += 8; // 4 bytes header + 4 bytes footer
    uint32 Is_Block_Found = 0;
	struct BlockElement *blk , *Found_Block;
	LIST_FOREACH(blk, &freeBlocksList)
	{
		//size that return from get block size is the (header + payload + footer) size
		if(get_block_size(blk) >= size)
		{
			Is_Block_Found = 1;
			Found_Block = blk;
			uint32 Block_Size = get_block_size(blk);//save the founded block size.
			LIST_REMOVE(&freeBlocksList, Found_Block);
			if((get_block_size(blk) == size) || ((get_block_size(blk) - size) <  16))
			{
				//make the block state allocated by call function set block data.
				set_block_data((void*)blk , size , 1);
			}
			else
			{
				//set the data for allocated block.
				set_block_data((void*)blk , size , 1);
				//remove it from the free block list.
				struct BlockElement* Helping_ptr;
				uint32 total_moves = (uint32)blk;
				total_moves += size;// header + footer + payload
				Helping_ptr = (struct BlockElement*)total_moves ;
				uint32 Reminder_size = Block_Size - size;
				set_block_data((void*)Helping_ptr , Reminder_size , 0);
				insert_block_sorted(Helping_ptr);
			}
			break;
		}
	}
	if(Is_Block_Found)
	{
		return (void *) blk;
	}
	else
	{
		    uint32 numOfPages = size / PAGE_SIZE; // Calculate required pages to call sbrk
			void *new_block_address = sbrk(numOfPages);
			if (new_block_address != (void *)-1)
			{
//				alloc_block_FF(size);
				return new_block_address;
			}
			else
			{
				// Failed to allocate more memory
				return NULL;
			}
	}
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...

}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("free_block is not implemented yet");
	//Your Code is Here...

	int fosvar = 0;
	int check= 0;

	    if (va == NULL) {   	    // Return if the address is NULL
	    	fosvar++;
	        return;
	    }

	    uint32 size_of_block = get_block_size(va);       // Find the block size
	    if (size_of_block == 0) {
	    	fosvar++;
	        return;
	    }

	    if (fosvar != -1) {
	        set_block_data(va, size_of_block, check);  //mark the block as free with false
	    }

	    void* next_blk = (void*)((char*)va + size_of_block);
	       int fosvar2= is_free_block(next_blk);
	       if (fosvar2) {
	           uint32 size_of_next_blk = get_block_size(next_blk);
	           size_of_block = size_of_block + size_of_next_blk; // Coalesce with next block
	           set_block_data(va, size_of_block, check);
	       }

	       struct BlockElement* cur_blk = (struct BlockElement*)va;
	           struct BlockElement* prev_blk = LIST_PREV(cur_blk);

	           if (prev_blk != NULL && is_free_block(prev_blk)) {
	               uint32 prev_block_size = get_block_size(prev_blk);
	               size_of_block += prev_block_size;
	               set_block_data(prev_blk, size_of_block, check);

	           }


}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
