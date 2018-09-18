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
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;

	if(proc-> priority + proc->nice + proc->queue_prio >= 119) {
   /* f4 */
	 proc->last_queue = 4;
	 enqueue(&f4, proc);
 } else if( proc-> priority + proc->nice + proc->queue_prio >= 80) {
   /* f3 */
	 proc->last_queue = 3;
	 enqueue(&f3, proc);
 } else if( proc-> priority + proc->nice + proc->queue_prio >= PRIO_USER) {
   /* f2 */
	 proc->last_queue = 2;
	 enqueue(&f2, proc);
 } else if( proc-> priority + proc->nice + proc->queue_prio >= PRIO_REGION) {
	 /* f1 */
	 proc->last_queue = 1;
	 enqueue(&f1, proc);
	} else {
		/*f0*/
		proc->last_queue = 0;
	  enqueue(&f0, proc);
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
	if (proc->state == PROC_STOPPED){
		switch(proc->last_queue) {
			case 4 :
         proc->queue_prio = 100 - proc->priority - proc->nice;
         break;
			case 3 :
         proc->queue_prio = 60 - proc->priority - proc->nice;
         break;
      case 2 :
				proc->queue_prio = 10 - proc->priority - proc->nice;
				break;
      case 1 :
				proc->queue_prio = -60 - proc->priority - proc->nice;
         break;
      default :
	  	break;
		}
		sched(proc);
	}

}

//Funcoes de aging das filas
PUBLIC void aging_f1(){
		if (isEmpty(&f1)) {
			return;
		}

		struct process* p = f1.primeiro_proc;
		struct process* l = f1.primeiro_proc;

		while (p != NULL) {

			p-> queue_prio += AGING_CONST;

			if (p-> priority + p->nice + p->queue_prio < -20) {
				if (f1.primeiro_proc == p) {
					f1.primeiro_proc = p->next;
				}
				else{
					l->next = p-> next;
				}
				sched(p);
			}
			else{
				l = p;
			}

			p = p->next;
		}

}

PUBLIC void aging_f2(){
		if (/* condition */isEmpty(&f2)) {
			/* code */
			return;
		}

		struct process* p = f2.primeiro_proc;
		struct process* l = f2.primeiro_proc;

		while (p != NULL) {

			p-> queue_prio += AGING_CONST;

			if (p-> priority + p->nice + p->queue_prio < -20) {
				if (f2.primeiro_proc == p) {
					f2.primeiro_proc = p->next;
				}
				else{
					l->next = p-> next;
				}
				sched(p);
			}
			else{
				l = p;
			}

			p = p->next;
		}


}
PUBLIC void aging_f3(){
		if (/* condition */isEmpty(&f3)) {
			/* code */
			return;
		}

		struct process* p = f3.primeiro_proc;
		struct process* l = f3.primeiro_proc;

		while (p != NULL) {

			p-> queue_prio += AGING_CONST;

			if (p-> priority + p->nice + p->queue_prio < -20) {
				if (f3.primeiro_proc == p) {
					f3.primeiro_proc = p->next;
				}
				else{
					l->next = p-> next;
				}
				sched(p);
			}
			else{
				l = p;
			}

			p = p->next;
		}

}

PUBLIC void aging_f4(){
		if (/* condition */isEmpty(&f4)) {
			/* code */
			return;
		}

		struct process* p = f4.primeiro_proc;
		struct process* l = f4.primeiro_proc;

		while (p != NULL) {

			p-> queue_prio += AGING_CONST;

			if (p-> priority + p->nice + p->queue_prio < -20) {
				if (f4.primeiro_proc == p) {
					f4.primeiro_proc = p->next;
				}
				else{
					l->next = p-> next;
				}
				sched(p);
			}
			else{
				l = p;
			}

			p = p->next;
		}

}


/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING){
		switch(curr_proc->last_queue) {
			case 3 :
         curr_proc->queue_prio = 140 - curr_proc->priority - curr_proc->nice;
         break;
      case 2 :
				curr_proc->queue_prio = 100 - curr_proc->priority - curr_proc->nice;
				break;
      case 1 :
				curr_proc->queue_prio = 60 - curr_proc->priority - curr_proc->nice;
         break;
      case 0 :
				curr_proc->queue_prio = 10 - curr_proc->priority - curr_proc->nice;
				break;
			default:
				break;
		}
		sched(curr_proc);
	}

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	/* Choose a process to run next. */
	next = IDLE;
	
	if(!isEmpty(&f0)) {
		next = dequeue(&f0);
		next->counter = PROC_QUANTUM - 20;

		//ageing
		aging_f1();
		aging_f2();
		aging_f3();
		aging_f4();
	} else if(!isEmpty(&f1)) {
		 next = dequeue(&f1);
		 next->counter = PROC_QUANTUM - 10;

		 aging_f2();
		 aging_f3();
		 aging_f4();
	 } else if(!isEmpty(&f2)) {
		 next = dequeue(&f2);
		 next->counter = PROC_QUANTUM;

		 aging_f3();
		 aging_f4();
	 } else if(!isEmpty(&f3)) {
		 next = dequeue(&f3);
		 next->counter = PROC_QUANTUM + 10;

		 aging_f4();
	 } else if(!isEmpty(&f4)) {
		 next = dequeue(&f4);
		 next->counter = PROC_QUANTUM + 20;
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
	next->state = PROC_RUNNING;
	switch_to(next);
}
