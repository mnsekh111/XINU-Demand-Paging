/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages) {

    STATWORD ps;
    disable(ps);

    // invalid inp
    if ((virtpage < 4096) || (source < 0) || (source > MAX_ID) || (npages < 1) || (npages > 128) || bsm_tab[source].bs_status == BSM_UNMAPPED || bsm_tab[source].bs_private == 1) {
        restore(ps);
        return SYSERR;
    } else {
        struct bs_proc_map_t *bs = (struct bs_proc_map_t*) getmem(sizeof (struct bs_proc_map_t));
        bs->pid = currpid;
        proctab[currpid].bstab[source] = (struct bs_map_t *) &bsm_tab[source];

        bs->vpageno = virtpage;
        bs->next = NULL;

        bs->next = bsm_tab[source].mapping;
        bsm_tab[source].mapping = bs;

        restore(ps);

        return OK;
    }
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage) {
   
    int i;
    STATWORD ps;
    disable(ps);
    struct bs_proc_map_t *temp;
    for (i = 0; i < NBS; i++) {
        temp = bsm_tab[i].mapping;
        while (temp != NULL) {
            if (temp->pid == currpid && temp->vpageno == virtpage && temp == bsm_tab[i].mapping) {
                if (remove_frames_for_xmunmap(i) == SYSERR) {
                    restore(ps);
                    return SYSERR;
                }
                temp = temp->next;
                bsm_tab[i].mapping = temp;
                restore(ps);
                return OK;
            } else if (temp->next != NULL) {
                if (temp->next->pid == currpid) {
                    temp->next = temp->next->next;
                    restore(ps);
                    return OK;
                }
            } else
                temp = temp->next;
        }
    }
    restore(ps);
    return SYSERR;
}


