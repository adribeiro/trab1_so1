/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>

/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc, int queue_p)
{
	proc->state = PROC_READY;
	proc->counter = 0;

	struct process_queue *p;
	p->process = proc;
	p->queue_priority = queue_p;

	if(proc-> priority + queue_p >= 119) {
   /* f4 */
	 enqueue(f4, p);
 } else if( proc-> priority + queue_p >= 80) {
   /* f3 */
	 enqueue(f3, p);
 } else if( proc-> priority + queue_p >= PRIO_USER) {
   /* f2 */
	 enqueue(f2, p);
 } else if( proc-> priority + queue_p >= PRIO_REGION) {
	 /* f1 */
	 enqueue(f1, p);
	} else {
   /*f0*/
	 enqueue(f0, p);
	}
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc, int x);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */
	struct process_queue *processo_queue;

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc,curr_prio + 50);

	/* Remember this process. */
	last_proc = curr_proc;


	// /* Check alarm. */
	// for (p = FIRST_PROC; p <= LAST_PROC; p++)
	// {
	// 	/* Skip invalid processes. */
	// 	if (!IS_VALID(p))
	// 		continue;
	//
	// 	/* Alarm has expired. */
	// 	if ((p->alarm) && (p->alarm < ticks))
	// 		p->alarm = 0, sndsig(p, SIGALRM);
	// }

	/* Choose a process to run next. */
	next = IDLE;

	if(isEmpty(!f0)) {
		processo_queue = dequeue(f0);
		next = processo_queue->processo;
		curr_prio = processo_queue->queue_priority;

		//ageing
		aging(f1);
		aging(f2);
		aging(f3);
		aging(f4);

 } else if(!isEmpty(f1)) {
	 processo_queue = dequeue(f1);
	 next = processo_queue->processo;
	 curr_prio = processo_queue->queue_priority;

	 aging(f2);
	 aging(f3);
	 aging(f4);

 } else if(!isEmpty(f2)) {
	 processo_queue = dequeue(f2);
	 next = processo_queue->processo;
	 curr_prio = processo_queue->queue_priority;

	 aging(f3);
	 aging(f4);
 } else if(!isEmpty(f3)) {
	 processo_queue = dequeue(f3);
	 next = processo_queue->processo;
	 curr_prio = processo_queue->queue_priority;

	 aging(f4);
 } else if(!isEmpty(f4)){
	 processo_queue = dequeue(f4);
	 next = processo_queue->processo;
	 curr_prio = processo_queue->queue_priority;
	}
	// for (p = FIRST_PROC; p <= LAST_PROC; p++)
	// {
	// 	/* Skip non-ready process. */
	// 	if (p->state != PROC_READY)
	// 		continue;
	//
	// 	/*
	// 	 * Process with higher
	// 	 * waiting time found.
	// 	 */
	// 	if (p->counter > next->counter)
	// 	{
	// 		next->counter++;
	// 		next = p;
	// 	}
	//
	// 	/*
	// 	 * Increment waiting
	// 	 * time of process.
	// 	 */
	// 	else
	// 		p->counter++;
	// }

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	switch_to(next);
}
