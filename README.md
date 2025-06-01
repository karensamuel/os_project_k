# FOS'24 Operating System Project

  

Enhance your OS development skills by building core components in the Faculty Operating System (FOS), a 32-bit x86 educational OS. This project consists of three milestones: MS1 (basic kernel features), MS2 (memory management and scheduling), and MS3 (page replacement, semaphores, and priority scheduling).
  
---
## 👥 Team Members

  
| Name             | GitHub Link      |
| ---------------- | -------------------- |
| Tasneem Mohamed Ahmed Mohamed         | https://github.com/Tasneem357Mohamed          |
| Bsmala Tarek Kamal Khalil Elbagoury         | https://github.com/Bsmalatarek          |
| Omnia Salah Mahmoud Hamed        | https://github.com/matata2020          |
| Karen Samuel Gerges Shenoda        | https://github.com/karensamuel          |
| Mennatullah Mohamed Ibrahim         | https://github.com/MennaHassaneen           |
| Mennatullah Khaled Ahmed Mohamed       |https://github.com/MennaKhalled           |


---
  

## 📑 Table of Contents


- [Milestone 1: Core Kernel Features](#milestone-1-core-kernel-features)
  - [Play with Code](#play-with-code)
  - [System Calls](#system-calls)
  - [Dynamic Allocator](#dynamic-allocator)
  - [Sleep Locks](#sleep-locks)
- [Milestone 2: Memory and Scheduling](#milestone-2-memory-and-scheduling)
  - [Kernel Heap](#kernel-heap)
  - [User Heap with Lazy Paging](#user-heap-with-lazy-paging)
  - [Page Fault Handling](#page-fault-handling)
  - [Shared Memory](#shared-memory)
  - [Multilevel Feedback Queue Scheduler](#multilevel-feedback-queue-scheduler)
- [Milestone 3: Page Replacement, Semaphores, and Priority Scheduling](#milestone-3-page-replacement-semaphores-and-priority-scheduling)
  - [Fault Handler II: Nth Chance Clock Replacement](#fault-handler-ii-nth-chance-clock-replacement)
  - [User-Level Semaphores](#user-level-semaphores)
  - [Priority Round-Robin Scheduler](#priority-round-robin-scheduler)
- [Overall Testing](#overall-testing)
- [Bonuses](#bonuses)
- [Helper Functions](#helper-functions)
  

---


# FOS'24 Operating System Project


Enhance your OS development skills by building core components in the Faculty Operating System (FOS), a 32-bit x86 educational OS. This project consists of three milestones: **MS1** (basic kernel features), **MS2** (memory management and scheduling), and **MS3** (page replacement, semaphores, and priority scheduling).


## Milestone 1: Core Kernel Features


Implement foundational kernel functionalities.


### Play with Code

- **Objective**: Familiarize with FOS using pointers and linked lists.
- **Task**: Implement `process_command` in `kern/cmd/command_prompt.c`.
  - Validates commands/arguments.
  - Returns: `CMD_INVALID` (invalid), `CMD_CONTINUING` (wrong args), `CMD_MATCHED` (matched), or index (valid).
- **Testing**:
  - `FOS> kernel_info`
  - `FOS> clk`
- **Files**: `kern/cmd/command_prompt.h`, `commands.h`, `commands.c`, `command_prompt.c`
  
### System Calls

- **Objective**: Add three system calls with parameter validation.
- **Tasks**: Implement in `lib/syscall.c`, `kern/trap/trap.c`, `kern/trap/syscall.c`, `inc/syscall.h`.
- **Testing**:
  - `FOS> run tst_syscalls_1 10`
  - `FOS> run tst_syscalls_2 10`
- **Note**: Uses Interrupt Descriptor Table (IDT) for kernel mode traps.

  
### Dynamic Allocator

- **Objective**: Build a First Fit (FF) memory allocator (optional Best Fit).
- **Tasks** (in `lib/dynamic_allocator.c`):
  - `initialize`: Set up data structures.
  - `set_block_data`: Configure metadata.
  - `alloc_block_FF`: Allocate memory.
  - `free_block`: Free and merge blocks.
  - `realloc_block_FF`: Resize blocks.
  - (Bonus) `alloc_block_BF`: Best Fit allocation.

- **Testing**:

  - `FOS> tst dynalloc init`
  - `FOS> tst dynalloc allocff`
  - `FOS> tst dynalloc freeff`
  - `FOS> tst dynalloc reallocff`
  - (Bonus) `FOS> tst dynalloc allocbf`, `FOS> tst dynalloc freebf`

- **Note**: Uses FOS LIST macros.

### Sleep Locks

- **Objective**: Implement sleep locks for synchronization.
- **Tasks** (in `kern/conc/channel.c`, `kern/conc/sleeplock.c`):
  - `sleep`: Block process on channel.
  - `wakeup_one`: Wake one process.
  - `wakeup_all`: Wake all processes.
  - `acquire_sleeplock`: Acquire lock or block.
  - `release_sleeplock`: Release lock, wake all.

- **Testing**:

  - **Modular**:
    - Sleep + Wakeup All: `FOS> run tst_chan_all 100` (input: 7 slaves)
    - Sleep + Wakeup One: `FOS> run tst_chan_one 100` (input: 7 slaves)
    - Acquire + Release: Unseen test (validate independently)

  - **Overall**:

    - Modify `kern/cons/console.h` to use `LCK_SLEEP` (not `LCK_INT`).
    - Run: `FOS> load mergesort`, `FOS> load fib`, `FOS> load fib`, `FOS> runall`
    - Input: First `fib` (35), second `fib` (38, wait 15s).
    - Expect: `mergesort` and first `fib` complete instantly; second `fib` after ~20s.

- **Given Functions**: `init_sleeplock`, `holding_sleeplock`, `get_cpu_proc`, `sched_insert_ready0`, `sched`

  

## Milestone 2: Memory and Scheduling


Develop advanced memory allocation and scheduling systems.

### Kernel Heap

- **Objective**: Implement `kmalloc`/`kfree` with two-layer allocator.
- **Layers**:
  - Page Allocator: ≥4KB requests (`[HARD_LIMIT + PAGE_SIZE, KERNEL_HEAP_MAX]`).
  - Block Allocator: ≤`DYN_ALLOC_MAX_BLOCK_SIZE` (`[KERNEL_HEAP_START, HARD_LIMIT]`).

- **Tasks** (in `kern/mem/kheap.h`, `kern/mem/kheap.c`):

  - `initialize_kheap`: Initialize allocator.
  - `sbrk`: Adjust segment break.
  - `kmalloc`, `kfree`: Allocate/free memory.
  - `kheap_virtual_address`, `kheap_physical_address`: Address conversion.
  - (Bonus) `krealloc`, Fast Page Allocator.

- **Testing**:

  - `FOS> tst kheap FF sbrk`
  - `FOS> tst kheap FF kmalloc 1`
  - `FOS> tst kheap FF fast`

- **Note**: Set `USE_KHEAP = 1` in `inc/memlayout.h`.


### User Heap with Lazy Paging

- **Objective**: Implement `malloc`/`free` with lazy allocation.
- **Tasks** (in `inc/environment_definitions.h`, `kern/proc/user_environment.c`, `lib/uheap.c`):
  - `initialize`: Set up heap.
  - `sys_sbrk`: Adjust heap size.
  - `malloc`, `free`: Allocate/free memory.
  - `allocate_user_mem`, `free_user_mem`: Manage memory.
  - (Bonus) O(1) `free_user_mem`.

- **Testing**:
  - `FOS> run tm1 3000`
  - `FOS> run tff2 10000`

  

### Page Fault Handling

- **Objective**: Handle page faults for lazy allocation and shared memory.
- **Tasks** (in `kern/trap/fault_handler.c`):
  - `page_fault_handler`: Allocate/map pages.
  - `page_fault_handler_with_buffering`: Buffer-based handling.

- **Testing**:

  - `FOS> run tpp 20`

  
### Shared Memory

- **Objective**: Enable shared memory between processes.

- **Tasks** (in `kern/mem/sharing.c`):

  - `create_shared_memory`, `get_shared_memory`: Manage regions.
  - `sys_createSharedObject`, `sys_getSharedObject`: System calls.

- **Testing**:

  - `FOS> run tshr1 3000`
  - `FOS> run tshr2 1000`

  

### Multilevel Feedback Queue Scheduler

- **Objective**: Implement a multi-queue scheduler.

- **Tasks** (in `kern/cpu/sched.c`):

  - `sched_init`: Initialize scheduler.
  - `scheduler`: Schedule processes.

- **Testing**:

  - `FOS> run tsched1`
  - `FOS> run tsched2`

  

## Milestone 3: Page Replacement, Semaphores, and Priority Scheduling

  

Extend FOS with advanced memory, synchronization, and scheduling features.

  

### Fault Handler II: Nth Chance Clock Replacement

- **Objective**: Implement Nth Chance Clock page replacement.

- **Task** (in `kern/trap/fault_handler.c`):

  - `page_fault_handler`: Normal (N chances) and Modified (N+1 for dirty pages).

- **Switching**:

  - `FOS> nclock <N> 1` (Normal)
  - `FOS> nclock <N> 2` (Modified)

  - Code: `setPageReplacementAlgorithmNchanceClock(int N)` in `kern/init.c`

- **Testing**:

  - `FOS> run tpr1 11`
  - `FOS> run tpr2 6`

  - `FOS> nclock 5 1; run tnclock1 11`
  - `FOS> nclock 5 2; run tnclock2 11`

  

### User-Level Semaphores

- **Objective**: Implement semaphores for synchronization.

- **Tasks** (in `lib/semaphore.c`, `inc/semaphore.h`):

  - `create_semaphore`: Initialize semaphore.
  - `get_semaphore`: Access semaphore.
  - `wait_semaphore`: Decrement count, block if <0.
  - `signal_semaphore`: Increment count, unblock if ≤0.

- **Testing**:

  - `FOS> run tsem1 100`
  - `FOS> run tsem2 100` (100 customers, shop capacity 30)
  - `FOS> run midterm 100`

  

### Priority Round-Robin Scheduler

- **Objective**: Implement Priority RR scheduler.
- **Tasks** (in `kern/cpu/sched.c`, `kern/cpu/sched_helpers.c`, `kern/cmd/commands.c`):
  - `set_process_priority_and_threshold`: Set priority/threshold.
  - `sched_init_PRR`: Initialize scheduler.
  - `scheduler`: Select process.
  - `timer_tick_handler`: Handle preemption.

- **Switching**:

  - `FOS> schedPRIRR <#priorities> <quantum> <starvThresh>`
  - `FOS> schedRR <quantum>`

- **Testing**:

  - `FOS> schedPRIRR 5 10 1000; load fib 100 0; load fib 100 4; runall`

  
### Overall Testing

- **Programs**:

  - `quicksort_noleakage.c` (qs1): No memory leaks.
  - `quicksort_leakage.c` (qs2): Memory leaks.
  - `mergesort_leakage.c` (ms2): Memory leaks.

- **Commands**:

  - Single: `FOS> run <prog> <WS Size> [<priority>]`
  - Multiple: `FOS> load <prog> <WS Size> <priority>; runall`

- **Scenarios**:

  - Vary WS sizes.
  - Compare Normal vs. Modified Nth Chance Clock.
  - Test leaky vs. non-leaky programs.
  - Evaluate scheduling with different priorities.

  

## Bonuses

- **Free Process (env_free)**:
  - V1: Free working set, page tables, stack, page file.
  - V2: Include shared objects/semaphores.
 
- **Efficient Nth Chance Clock**: O(N) complexity.

- **FOS Enhancement**: Propose performance/security improvements.

  

## Helper Functions

- **MS1**: LIST macros (`LIST_INIT`, `LIST_INSERT`), string functions.

- **MS2**: `map_frame`, `unmap_frame`, `allocate_frame`, `free_frame`.

- **MS3**: Page file helpers, `fos_schedule_PRIRR`, `create_page_table`.
