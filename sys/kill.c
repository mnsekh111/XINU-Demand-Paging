/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev,i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);
	remove_frames_for_kill(pid);
	for(i=0 ; i<NBS ; i++){
		//kprintf("i = %d\n",i);
	  if(bsm_tab[i].bs_status == BSM_MAPPED){
		  //kprintf("i in kill = %d\n",i);
		struct bs_proc_map_t *temp = bsm_tab[i].mapping;
		while(temp != NULL){
			if((temp->pid == pid) && (temp == bsm_tab[i].mapping)){
				temp = temp->next;
				//if(temp == NULL)
				bsm_tab[i].mapping = temp;
				break;
			}
			else if(temp->next != NULL){
				if(temp->next->pid == pid){
					temp->next = temp->next->next;
					break;
				}
			}
			temp = temp->next;
		}
		if(bsm_tab[i].mapping == NULL){
			bsm_tab[i].bs_status = BSM_UNMAPPED;
			free_bsm(i);
		}
			
		/*
		if(bsm_tab[i].mapping == NULL){
			bsm_tab[i].bs_status = BSM_UNMAPPED;
			bsm_tab[i].mapping = NULL;
			bsm_tab[i].bs_npages = -1;
		}*/
	  }
    }
	//kprintf("12\n");
	//kprintf("killing pid %d \n",pid);
	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
	//kprintf("14\n");
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;
//kprintf("15\n");
	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:	//kprintf("killing pid %d in sleep\n",pid);
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
