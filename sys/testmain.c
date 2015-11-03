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
    int i; 
    get_bs(4, 100); 
    xmmap(7000, 4, 100);    /* This call simply creates an entry in the backing store mapping */ 
    x = (char*)(7000*4096); 
  for (i = 0; i < 26; i++) {
    *(x + i * NBPG) = 'A' + i;
  }
//sleep(3);
  for (i = 0; i < 26; i++) {
    kprintf("0x%08x: %c\n", x + i * NBPG, *(x + i * NBPG));
  }
  //sleep(3);
	xmunmap(7000);
	release_bs(4);
}
void procB(char c){
	char *x; 
    int i	; 
	kprintf("get bs in proc B returened = %d\n",get_bs(4,100));
	if(xmmap(6000, 4, 100) == SYSERR){
		sleep(3); 
		return ;
	}
	x = (char*)(6000*4096); 
  for (i = 0; i < 26; i++) {
    kprintf("0x%08x: %c\n", x + i * NBPG, *(x + i * NBPG));
  }
	xmunmap(6000);
	release_bs(4);
	return ;
	
}
int procC(char c){
	kprintf("\nIn Process procC\n");
	
}
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	srpolicy(LRU);
	resume(prA = create(procA, 2000, 40, "proc A", 1, 'A'));
	resume(prB = create(procB, 2000, 30, "proc B", 1, 'B'));
	return 0;
}
