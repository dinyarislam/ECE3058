/*
 * student.c
 * Multithreaded OS Simulation for CS 2200 and ECE 3058
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);
pcb_t* ready_queue();
void enqueue(pcb_t* start);
pcb_t* dequeue();


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t curr_mutex;
static pthread_mutex_t queue_mutex;
static pthread_cond_t thread_cond;
static pcb_t* start;
static int time_slice; 
static unsigned int cpu_num;
static int scheduler_algo; // type of scheduling algorigthm: 0.FIFO 1.RoundRobin 2.LRTF

/*
* Method to implement the ready queue
*/
pcb_t* ready_queue() {
    pthread_mutex_lock(&queue_mutex);
    pcb_t* block_pointer;

    if (start == NULL) {
        pthread_mutex_unlock(&queue_mutex);
        return start;
    } else {
        block_pointer = start;
        pcb_t* curr_pointer = start;
        while (curr_pointer != NULL) {
            if (block_pointer->time_remaining > curr_pointer->time_remaining) {
                block_pointer = curr_pointer;
            }
            curr_pointer = curr_pointer->next;
        }
    }

    pcb_t* temp_pointer = start;
    if (block_pointer->pid == start->pid) {
        start = start->next;
    } else {
        while (temp_pointer->next->pid != block_pointer->pid) {
            temp_pointer = temp_pointer->next;
        }
        temp_pointer->next = block_pointer->next;
    }
    pthread_mutex_unlock(&queue_mutex);
    return block_pointer;
}

/*
* Method to enqueue process into the ready queue
*/
void enqueue(pcb_t* process) {
    pthread_mutex_lock(&queue_mutex);
    pcb_t* curr_pointer;
    process->next = NULL;

    if (start == NULL){
        start = process;
    } else {
        curr_pointer = start;
        while (curr_pointer->next != NULL) {
            curr_pointer = curr_pointer->next;
        }
        curr_pointer->next = process;
    }

    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&thread_cond);
}

/* 
* Method to dequeue process from the ready queue
*/
extern pcb_t* dequeue(void)
{
    pthread_mutex_lock(&queue_mutex);
    pcb_t* curr_pointer;

    if (start == NULL){
        pthread_mutex_unlock(&queue_mutex);
        return start;
    } else {
        curr_pointer = start;
        start = start->next;
        curr_pointer->next = NULL;
    }
    
    pthread_mutex_unlock(&queue_mutex);
    return curr_pointer;
}

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    pcb_t* curr_pointer;
    if (scheduler_algo == 2) {
        curr_pointer = ready_queue();
    } else {
        curr_pointer = dequeue();
    }

    if (curr_pointer == NULL) {
        context_switch(cpu_id, NULL, time_slice);
    } else {
        curr_pointer->state = PROCESS_RUNNING;
        pthread_mutex_lock(&curr_mutex);
        current[cpu_id] = curr_pointer;
        pthread_mutex_unlock(&curr_mutex);
        context_switch(cpu_id, curr_pointer, time_slice);
    }
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&queue_mutex);
    while (start == NULL) {
        pthread_cond_wait(&thread_cond, &queue_mutex);
    }
    pthread_mutex_unlock(&queue_mutex);
    schedule(cpu_id);
}

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    //mt_safe_usleep(1000000);


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&curr_mutex);
    pcb_t* curr_pointer = current[cpu_id];
    curr_pointer->state = PROCESS_READY;
    enqueue(curr_pointer);
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    pthread_mutex_lock(&curr_mutex);
    current[cpu_id]->state = PROCESS_WAITING;
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&curr_mutex);
    current[cpu_id]->state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is LRTF, wake_up() may need
 *      to preempt the CPU with lower remaining time left to allow it to
 *      execute the process which just woke up with higher reimaing time.
 * 	However, if any CPU is currently running idle,
* 	or all of the CPUs are running processes
 *      with a higher remaining time left than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    process->state = PROCESS_READY;
    enqueue(process);
    
    if (scheduler_algo == 2) {
        unsigned int preempt_cpu_id;
        unsigned int shortest_time = 100000000000000000;
        pthread_mutex_lock(&curr_mutex);
        for (unsigned int i = 0; i < cpu_num; i++) {
            if (current[i] == NULL) {
                pthread_mutex_unlock(&curr_mutex);
                return;
            } else {
                if (current[i]->time_remaining < shortest_time) {
                    shortest_time = current[i]->time_remaining;
                    preempt_cpu_id = i;
                }
            }
        }
        pthread_mutex_unlock(&curr_mutex);
        
        if (shortest_time < process->time_remaining) { 
            force_preempt(preempt_cpu_id);
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -l and -r command-line parameters.
 */
int main(int argc, char *argv[])
{
    unsigned int cpu_count;


    /* Parse the command line arguments and validation checks for num of args*/
    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -l | -r <time slice> ]\n"
            "    Default : FIFO Scheduler\n"
	    "         -l : Longest Remaining Time First Scheduler\n"
            "         -r : Round-Robin Scheduler\n\n");
        return -1;
    }

    cpu_count = strtoul(argv[1], NULL, 0);

    /* FIX ME - Add support for -r and -l parameters*/
    char parameters;
    scheduler_algo = 0;
    time_slice = -1;

    if (argc > 2) {
        parameters = *((argv[2]) + 1);
        if (parameters == 'l') {
            scheduler_algo = 2;
        }
        else if (parameters == 'r') {
            scheduler_algo = 1;
            time_slice = strtoul(argv[3], NULL, 0);
        }
    }
    
    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&curr_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}