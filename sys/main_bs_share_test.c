/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <stdio.h>
#include <paging.h>
#include <proc.h>

void halt();
int prA, prB, prC;
int procA(char c){
	char *x; 
    char temp; 
    get_bs(4, 100); 
    xmmap(7000, 4, 100);    /* This call simply creates an entry in the backing store mapping */ 
    x = 7000*4096; 
	*x = 'C';
	kprintf("in process A at first temp = %c\n",*x);
    *x = 'F';
	temp = *x;
	kprintf("in process A temp = %c\n",temp);
	sleep(10);
	//xmunmap(7000);
	//release_bs(4);
}
int procB(char c){
	char *x; 
    char temp_b; 
	kprintf("get bs in proc B returened = %d\n",get_bs(4,100));
    xmmap(6000, 4, 100); 
    x = 6000 * 4096; 
    temp_b = *x;
	kprintf("in process B temp_b = %c\n",temp_b);
	xmunmap(6000);
	release_bs(4);
	
}
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf(" Starting main\n");
	unsigned long cr0_regvalue;
	int *a = 0x1b59044;
	int store_id,pg_off;
	int pgs;
	//kprintf("&a = 0x%x\n",&a);
	pgs = get_bs(3,50);
	if(pgs == SYSERR)
		kprintf("get_bs failled with syserr\n");
	else
		kprintf("get_bs success with npages = %d\n", pgs);
	if(xmmap(7000,3,50) == SYSERR){
		kprintf("xmmap failled with syserr\n");
	}

	*a = 2000;
	kprintf("a = 0x%x\n",a);
	
	resume(prA = create(procA, 2000, 90, "proc A", 1, 'A'));
	resume(prB = create(procB, 2000, 50, "proc B", 1, 'B'));
	kprintf("using vcreate to create process C\n");
	resume(prC = vcreate(procB, 2000,100, 40, "proc C", 1, 'C'));
	return 0;
}
