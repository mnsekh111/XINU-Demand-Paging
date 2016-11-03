/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
	popl pferrcode
	pushfl  //push flag registers
	cli  //disable interrupts
	pushal // push general registers
	call pfint
	popal
	popfl
	iret


