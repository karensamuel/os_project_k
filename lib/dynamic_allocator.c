/* dynamic_allocator.c
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
void insert_block_sorted(struct BlockElement *newtas_block)
{
    struct BlockElement *cuuurrentsk;
    LIST_FOREACH(cuuurrentsk, &freeBlocksList) {
        if (newtas_block < cuuurrentsk)
        {
            LIST_INSERT_BEFORE(&freeBlocksList,cuuurrentsk, newtas_block);
            return;
        }
    }
    LIST_INSERT_TAIL(&freeBlocksList, newtas_block);
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
	uint32 *Beginkar_Block , *Endmen_Block;
	//create Begin Block
	Beginkar_Block= (uint32 *)daStart;
	*Beginkar_Block = 1;

	//create End Block

	uint32 Heap_Limitomn = daStart + initSizeOfAllocatedSpace - 4  ;
	Endmen_Block = (uint32 *)Heap_Limitomn;
	*Endmen_Block = 1;

//	uint32 *Header_Ptr = Begin_Block;
//	Header_Ptr += 1; //take space for header and size for Begin.

	uint32 *headerbas = Beginkar_Block  + 1;
	uint32 *footertas =  (uint32 *)(daStart + initSizeOfAllocatedSpace - 8) ;
	*headerbas = initSizeOfAllocatedSpace - 8;
	*footertas= initSizeOfAllocatedSpace - 8;
	struct BlockElement * Blockmenna= (struct BlockElement*)(headerbas +1);
	LIST_INIT(&freeBlocksList);
	LIST_INSERT_HEAD(&freeBlocksList,Blockmenna);// in head of list(first Element)
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

	uint32 * bloockomnHiader = (uint32 *)va - 1;//reach00 header

	uint32 totally_basmoves = (uint32) va + (totalSize - 8) ;


	//reach to size of footer.
//	va = (void *)((char *)va + totalSize);
	//ptr point to footer
	uint32 * blooockarFooter = (uint32 *)totally_basmoves;
	if(isAllocated == 1 && get_block_size(va) % 2 == 0)
	{
		//block is allocated (lsb = 1).
		totalSize++;
	}
	else if(isAllocated == 0 && get_block_size(va) % 2 != 0)
	{
		///kkkk
		totalSize--;
	}
	*bloockomnHiader = totalSize;
	*blooockarFooter = totalSize;
	//for check only
	//cprintf("The Block Size: %d, My Block Free? %d\n", get_block_size(va), is_free_block(va));
}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
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

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...
    size += 8; // 4 bytes header + 4 bytes footer
    uint32 Iss_Blockar_Foundmen = 0;
	struct BlockElement *blkarmen , *Founddd_Blockar;
	LIST_FOREACH(blkarmen, &freeBlocksList)
	{
		//al size al birg3 mn  get block size hoa al (header + payload + footer) size
		if(get_block_size(blkarmen) >= size)
		{
			Iss_Blockar_Foundmen = 1;
			Founddd_Blockar = blkarmen;
			uint32 Block_Size = get_block_size(blkarmen);//save the founded block size.
			if((get_block_size(blkarmen) == size) || ((get_block_size(blkarmen) - size) <  16))
			{
				//5aly al block state allocated 3mtri2 call function set block data.
				set_block_data((void*)blkarmen , get_block_size(blkarmen) , 1);
			}
			else
			{
				//set the data for allocated block.
				set_block_data((void*)blkarmen , size , 1);
				//remove it from the free block list.
				struct BlockElement* Helping_ptr;
				uint32 total_moves = (uint32)blkarmen;
				total_moves += size;// header + footer + payload
				Helping_ptr = (struct BlockElement*)total_moves ;
				uint32 Reminder_size = Block_Size - size;

				set_block_data((void*)Helping_ptr , Reminder_size , 0);
				insert_block_sorted(Helping_ptr);
			}
			LIST_REMOVE(&freeBlocksList, Founddd_Blockar);
			break;
		}
	}
	if(Iss_Blockar_Foundmen)
	{
		return (void *) blkarmen;
	}
	else
	{
		    uint32 numOfPages = size / PAGE_SIZE; // a7sb required pages to call sbrk
			void *new_block_address = sbrk(numOfPages);
			if (new_block_address != (void *)-1)
			{
//				alloc_block_FF(size);
				return alloc_block_FF(size);
			}
			else
			{
				// Fashl en y allocate more memory
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
//	panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...
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
//	cprintf("1\n");
		uint32 bestttt_sizekkk = UINT_MAX;
		struct BlockElement *blkkk = LIST_FIRST(&freeBlocksList);
		struct BlockElement *bsttt_blkkkk;
		int Find_Free_Block = 0;
		//kkkkk
		size += 2 * (sizeof(uint32));
		LIST_FOREACH(blkkk, &freeBlocksList)
		{
//			cprintf("2\n");
			if(get_block_size(blkkk) >= size)
		   {
//				cprintf("3\n");
				if(get_block_size(blkkk) < bestttt_sizekkk)
				{
					Find_Free_Block = 1;
//					cprintf("4\n");
					bestttt_sizekkk = get_block_size(blkkk);
//					cprintf("5\n");
					bsttt_blkkkk = blkkk;
				}
		   }
//			cprintf("6\n");
		}
		if(Find_Free_Block)
		{
//			cprintf("7\n");
			uint32 add = (uint32)bsttt_blkkkk;

			LIST_REMOVE(&freeBlocksList,(struct BlockElement *)add);
//			cprintf("8\n");
			if((get_block_size(bsttt_blkkkk) == size) || ((get_block_size(bsttt_blkkkk) - size) <  16))
			{
//				cprintf("9\n");
				set_block_data((void*)bsttt_blkkkk, get_block_size(bsttt_blkkkk) , 1);
			}
			else
			{
//				cprintf("10\n");
				//set the data for allocated block.
				set_block_data((void*)bsttt_blkkkk, size , 1);
				//remove it from the free block list.
				struct BlockElement* Helping_ptr;
				uint32 total_moves = (uint32)bsttt_blkkkk;
				total_moves += size;// header + footer + payload
				Helping_ptr = (struct BlockElement*)total_moves ;
				uint32 Reminder_size = bestttt_sizekkk - size;
				set_block_data((void*)Helping_ptr , Reminder_size , 0);
				insert_block_sorted(Helping_ptr);
//				cprintf("11\n");
			}
//			cprintf("12\n");
			return (void*)bsttt_blkkkk;
		}
		else
		{
			return NULL;
			 uint32 numOfPages = size / PAGE_SIZE;
			void *new_block_address = sbrk(numOfPages);
			if (new_block_address != (void *)-1)
			{
//
				return alloc_block_BF(size);
			}
			else
			{
				// Failed to allocate more memory
				return NULL;
			}
		}
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...
	//mm
	if(va==NULL||is_free_block(va)==1)return;

	else
	{
		uint32 currentkar_block_sizeee=get_block_size(va);
		uint32 movesss_tol_next=(uint32)va+currentkar_block_sizeee;
		uint32*nextas_prt=(uint32*)movesss_tol_next;
		uint32 moves_to_prv=(uint32)va-8;
		uint32*prv_prt=(uint32*)moves_to_prv;
		uint32 flag=(~(*prv_prt) & 0x1) ;

			 if(is_free_block(nextas_prt)==0 && flag==0)
			 {
			set_block_data(va,currentkar_block_sizeee,0);
//			struct BlockElement*me=(struct BlockElement*)va;
			insert_block_sorted((struct BlockElement*)va);

			 }
			 else if(is_free_block(nextas_prt)==1&& flag==0){
				 uint32 next_block_size=get_block_size(nextas_prt);
				 LIST_REMOVE(&freeBlocksList,(struct BlockElement*)nextas_prt);
				 set_block_data(va,(next_block_size+currentkar_block_sizeee),0);
				 insert_block_sorted((struct BlockElement*)va);

			 }
			 else if(is_free_block(nextas_prt)==0&& flag==1)
			 {
				 	uint32 prv_block_size= (*prv_prt)& ~(0x1);
				 	uint32 payload_foot =  (uint32)va;
				 	uint32*prv_header_ptr=(uint32*)(payload_foot-(prv_block_size));
				 	 LIST_REMOVE(&freeBlocksList,(struct BlockElement*)prv_header_ptr);
				 	set_block_data(prv_header_ptr,(prv_block_size+currentkar_block_sizeee),0);
				 	insert_block_sorted((struct BlockElement*)prv_header_ptr);

			 }
			 else{
				 uint32 next_block_size=get_block_size(nextas_prt);
				 uint32 prv_block_size= (*prv_prt)& ~(0x1);
				 uint32 payload_foot =  (uint32)va;
				 uint32*prv_header_ptr=(uint32*)(payload_foot-(prv_block_size));
				 LIST_REMOVE(&freeBlocksList,(struct BlockElement*)prv_header_ptr);
				 LIST_REMOVE(&freeBlocksList,(struct BlockElement*)nextas_prt);
				 set_block_data(prv_header_ptr,(prv_block_size+(currentkar_block_sizeee+next_block_size)),0);
				 insert_block_sorted((struct BlockElement*)prv_header_ptr);
			 }

	}
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void *va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...

    if(new_size==0 && va!=NULL)
    {


    	free_block(va);

    	return NULL;
    }
    else if(va==NULL && new_size != 0)
    {

    	return alloc_block_FF(new_size);

    }
    else if(va==NULL && new_size==0)
    {

    	return NULL;

    }
	uint32 oldy_siz = get_block_size(va);
    new_size+=8;
	if(new_size == oldy_siz)
	{

		return va;
	}

	uint32 nextas_headeer = (uint32) va + oldy_siz ;
	uint32 *point_nex_header = (uint32 *)nextas_headeer;

	uint32 size_nex_block = get_block_size(point_nex_header);



	if(new_size > oldy_siz)
	{
		cprintf("cond1\n");
		if((is_free_block(point_nex_header)) && ((oldy_siz + size_nex_block) >= new_size)) //if size even then next block is free
		{

			cprintf("cond2\n");
			 LIST_REMOVE(&freeBlocksList, (struct BlockElement*)point_nex_header);
			 cprintf("5\n");
				if(((oldy_siz + size_nex_block)-(new_size)) < 16 || ((oldy_siz + size_nex_block)-(new_size)) == 0)  //internal fragmentation
				  {
					  cprintf("cond3\n");
						//uint32 new_footer = old_footer + size_nex_block ;
						//uint32 *point_new_footer = (uint32 *)new_footer;


					    //intern_frg = (old_size + size_nex_block)-(new_size);
						set_block_data((void *)va,(oldy_siz + size_nex_block) ,1);
						cprintf("done3\n");
				   }
				 else
				 {
						//uint32 new_footer = (uint32)va + (new_size -8) ; // va + payload for new block
						//uint32 *point_new_footer = (uint32 *)new_footer;
					    uint32 moves = (uint32) va + new_size ;
					    struct BlockElement* point_newblock = (struct BlockElement*)moves;
					    cprintf("cond6\n");
						set_block_data(va,new_size,1);
						cprintf("cond7\n");
						set_block_data((void *)moves,(size_nex_block + oldy_siz)- new_size,0);
						cprintf("cond8\n");
						insert_block_sorted(point_newblock);
						cprintf("cond9\n");

				  }

		  }

		else if(!is_free_block(point_nex_header) || (((oldy_siz + size_nex_block) < new_size) && is_free_block(point_nex_header)) )
		{
			cprintf("cond10\n");
			free_block(va);
			return alloc_block_FF(new_size);
			cprintf("cond11\n");
		}
		else if(LIST_SIZE(&freeBlocksList) == 0)

		{
			uint32 numOfPages = new_size / PAGE_SIZE; // Calculate required pages to call sbrk
									void *new_block_address = sbrk(numOfPages);
									if (new_block_address != (void *)-1)
									{
						//				alloc_block_FF(size);
										return alloc_block_FF(new_size);
									}
									else
									{
										// Failed to allocate more memory
										return NULL;
									}
			cprintf("cond12\n");
			return NULL;
		}
	}
	else          //new size 22l mn al old
	{
		cprintf("cond13\n");



		if( is_free_block(point_nex_header))
		   {
			         uint32 moves = (uint32) va + new_size  ;
					 struct BlockElement* point_newblock = (struct BlockElement*)moves;
					cprintf("cond18\n");
					LIST_REMOVE(&freeBlocksList, (struct BlockElement*)point_nex_header);
					 set_block_data((void *)point_newblock,(oldy_siz - new_size)+size_nex_block,0);
					 set_block_data(va,new_size,1);
					 cprintf("cond19\n");
					 insert_block_sorted(point_newblock);
					 cprintf("cond20\n");


		  }

		else
		{
			//uint32 new_footer = (uint32)va + (new_size -8) ; // va + payload for new block
			//uint32 *point_new_footer = (uint32 *)new_footer;
			if((oldy_siz - new_size) < 16 || ((oldy_siz - new_size))==0)    //internal fragmentation
			{
				cprintf("cond14\n");
				//uint32 *point_new_footer = (uint32 *)old_footer;
	//			intern_frg = (old_size - new_size);
				set_block_data(va,oldy_siz,1);
				cprintf("cond15\n");
			}
			else
			{
				uint32 moves = (uint32) va + new_size  ;   //after header for new free block
				struct BlockElement* point_newblock = (struct BlockElement*)moves;
				cprintf("cond16\n");
				set_block_data(va,new_size ,1);
				cprintf("cond18\n");
				set_block_data((void *)point_newblock,(oldy_siz - new_size),0);
				cprintf("cond19\n");
				insert_block_sorted(point_newblock);
				cprintf("cond20\n");

			}


		}
	}
	return va;
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
