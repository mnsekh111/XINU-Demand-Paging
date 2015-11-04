/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];

/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL vfreemem(block, size)
struct mblock *block;
unsigned size;
{
    STATWORD ps;
    struct mblock *p, *q;
    unsigned top;

    struct mblock *vmemlist = proctab[currpid].vmemlist;
    
    //Check if the block of size (size) is within the heap range
    
    if (size == 0 || (unsigned) block > (unsigned) (((proctab[currpid].vhpno)+(proctab[currpid].vhpnpages)) * NBPG)
            || ((unsigned) block)<((unsigned) (proctab[currpid].vhpno) * NBPG))
        return (SYSERR);
    
    
    size = (unsigned) roundmb(size);
    disable(ps);
    
    
    for (p = vmemlist->mnext, q = vmemlist;
            p != (struct mblock *) NULL && p < block;
            q = p, p = p->mnext)
        ;
    
    // check if memory is valid or not
    if (((top = q->mlen + (unsigned) q)>(unsigned) block && q != vmemlist) ||
            (p != NULL && (size + (unsigned) block) > (unsigned) p)) {
        restore(ps);
        return (SYSERR);
    }
    
    // check if memory is available at the top of free list
    if (q != vmemlist && top == (unsigned) block)
        q->mlen += size;
    else { // place the block at the top or beginning.]
        block->mlen = size;
        block->mnext = p;
        q->mnext = block;
        q = block;
    }
    if ((unsigned) (q->mlen + (unsigned) q) == (unsigned) p) {
        q->mlen += p->mlen;
        q->mnext = p->mnext;
    }
    restore(ps);
    return (OK);
}
