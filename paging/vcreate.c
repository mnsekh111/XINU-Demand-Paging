/* vcreate.c - vcreate */

#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
 */
struct mblock vmmemlist;
LOCAL newpid();

/*------------------------------------------------------------------------
 *  vcreate  -  creates a process with private heap
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr, ssize, hsize, priority, name, nargs, args)
int *procaddr; /* procedure address		*/
int ssize; /* stack size in words		*/
int hsize; /* virtual heap size in pages	*/
int priority; /* process priority > 0		*/
char *name; /* name (for debugging)		*/
int nargs; /* number of args that follow	*/
long args; /* arguments (treated like an	*/
/* array in the code)		*/
{
    unsigned long savsp, *pushsp;
    STATWORD ps;
    int pid; /* stores new process id	*/
    struct pentry *pptr; /* pointer to proc. table entry */
    int i;
    unsigned long *a; /* points to list of args	*/
    unsigned long *saddr; /* stack address		*/
    int INITRET();
    
    struct mblock *mptr;

    disable(ps);
    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (int) roundew(ssize);
    if (((saddr = (unsigned long *) getstk(ssize)) ==
            (unsigned long *) SYSERR) ||
            (pid = newpid()) == SYSERR || priority < 1 || hsize < 1 || hsize > 128) {
        restore(ps);
        return (SYSERR);
    }

    int new_frame;
    bsd_t bs;
    
    if (get_bsm(&bs) == SYSERR) {
        restore(ps);
        return (SYSERR);
    }

    if (get_bs(bs, hsize) == SYSERR) {
        restore(ps);
        return (SYSERR);
    }
    numproc++;
    pptr = &proctab[pid];

    pptr->fildes[0] = 0; /* stdin set to console */
    pptr->fildes[1] = 0; /* stdout set to console */
    pptr->fildes[2] = 0; /* stderr set to console */

    for (i = 3; i < _NFILE; i++) /* others set to unused */
        pptr->fildes[i] = FDFREE;

    pptr->pstate = PRSUSP;
    for (i = 0; i < PNMLEN && (int) (pptr->pname[i] = name[i]) != 0; i++)
        ;
    pptr->pprio = priority;
    pptr->pbase = (long) saddr;
    pptr->pstklen = ssize;
    pptr->psem = 0;
    pptr->phasmsg = FALSE;
    pptr->plimit = pptr->pbase - ssize + sizeof (long);
    pptr->pirmask[0] = 0;
    pptr->pnxtkin = BADPID;
    pptr->pdevs[0] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;


    get_frm(&new_frame);
    frm_tab[new_frame].fr_type = FR_DIR;
    frm_tab[new_frame].fr_pid = pid;

    pptr->pdbr = (new_frame + FRAME0) * NBPG;
    pd_t *ptr_t = pptr->pdbr;
    
    //Base pdbr
    pd_t *ptr_d = proctab[NULLPROC].pdbr;


    for (i = 0; i < 4; i++) {
        add_page_dir(ptr_t, ptr_d->pd_base, new_frame);
        ptr_t++;
        ptr_d++;
    }

    for (i = 0; i < NBS; i++) {
        pptr->bstab[i] = NULL;
    }

    //Specifies that it is having private heap
    bsm_tab[bs].bs_private = 1;
    struct bs_proc_map_t *bsm = (struct bs_proc_map_t*) getmem(sizeof (struct bs_proc_map_t));
    bsm->pid = pid;

    //Heap grows from lowest address - lowest cit address = 4096
    bsm->vpageno = 4096;

    bsm->next = NULL;
    bsm->next = bsm_tab[bs].mapping;
    bsm_tab[bs].mapping = bsm;

    proctab[currpid].bstab[bs] = (struct bs_map_t *) &bsm_tab[bs];

    pptr->store = bs;
    pptr->vhpno = 4096;
    pptr->vhpnpages = hsize;
    pptr->vmemlist = &vmmemlist;

    int temppid = currpid;
    currpid = pid;
    write_cr3(pptr->pdbr);


    vmmemlist.mnext = mptr = (struct mblock *) roundmb((pptr->vhpno) * NBPG);

    mptr->mnext = 0;
    mptr->mlen = (int) truncew((unsigned) (((pptr->vhpno) + hsize) * NBPG) - (unsigned) ((pptr->vhpno) * NBPG));

    currpid = temppid;
    write_cr3(proctab[currpid].pdbr);
    /* Bottom of stack */
    *saddr = MAGIC;
    savsp = (unsigned long) saddr;

    /* push arguments */
    pptr->pargs = nargs;
    a = (unsigned long *) (&args) + (nargs - 1); /* last argument	*/
    for (; nargs > 0; nargs--) /* machine dependent; copy args	*/
        *--saddr = *a--; /* onto created process' stack	*/
    *--saddr = (long) INITRET; /* push on return address	*/

    *--saddr = pptr->paddr = (long) procaddr; /* where we "ret" to	*/
    *--saddr = savsp; /* fake frame ptr for procaddr	*/
    savsp = (unsigned long) saddr;

    /* this must match what ctxsw expects: flags, regs, old SP */
    /* emulate 386 "pushal" instruction */
    *--saddr = 0;
    *--saddr = 0; /* %eax */
    *--saddr = 0; /* %ecx */
    *--saddr = 0; /* %edx */
    *--saddr = 0; /* %ebx */
    *--saddr = 0; /* %esp; fill in below */
    pushsp = saddr;
    *--saddr = savsp; /* %ebp */
    *--saddr = 0; /* %esi */
    *--saddr = 0; /* %edi */
    *pushsp = pptr->pesp = (unsigned long) saddr;

    restore(ps);

    return (pid);

}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL newpid() {
    int pid; /* process id to return		*/
    int i;

    for (i = 0; i < NPROC; i++) { /* check all NPROC slots	*/
        if ((pid = nextproc--) <= 0)
            nextproc = NPROC - 1;
        if (proctab[pid].pstate == PRFREE)
            return (pid);
    }
    return (SYSERR);
}
