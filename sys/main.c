/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	int * a = 4000*NBPG;
	*a = 10;

	kprintf("Valid access %d",*a);

	kprintf("Main process pdbr %lu",proctab[currpid].pdbr);


	//Checking if global page tables are the same
	int i;
	pd_t * ptr = (pd_t*)proctab[currpid].pdbr;
	pd_t * ptr2 = (pd_t*)proctab[NULLPROC].pdbr;
	for(i =0;i<4;i++){
		kprintf("%u %u\n",ptr->pd_base,ptr2->pd_base);

		ptr++;
		ptr2++;
	}

//      Invalid access
//	a = 4097 * NBPG;
//	*a = 15;
	return 0;
}
