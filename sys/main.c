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


	// invalid access
//	a = 4097 * NBPG;
//	*a = 15;
	return 0;
}
