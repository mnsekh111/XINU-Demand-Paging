/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm() {
    STATWORD ps;
    disable(ps);
    int i;
    for (i = 0; i < NBS; i++) {
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        bsm_tab[i].mapping = NULL;
        bsm_tab[i].bs_npages = -1;
        bsm_tab[i].bs_private = 0;
    }
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail) {
    int i;
    STATWORD ps;
    disable(ps);
    for (i = 0; i <= MAX_ID; i++) {
        if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
            *avail = i;
            restore(ps);
            return OK;
        }
    }
    restore(ps);
    return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i) {
    STATWORD ps;
    disable(ps);
    bsm_tab[i].bs_status = BSM_UNMAPPED;
    bsm_tab[i].mapping = NULL;
    bsm_tab[i].bs_npages = -1;
    bsm_tab[i].bs_private = 0;
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {
    STATWORD ps;
    disable(ps);
    int vpage;
    int i, vpage_start, vpage_end;
    vpage = (vaddr & ~(NBPG - 1)) >> 12;
    vpage = vpage & 0x000FFFFF;
    struct bs_proc_map_t *head;
    for (i = 0; i < NBS; i++) {
        if (bsm_tab[i].bs_status == BSM_MAPPED) {
            head = bsm_tab[i].mapping;
            while (head != NULL) {
                if (head->pid == pid) {
                    vpage_start = head->vpageno;
                    vpage_end = vpage_start + bsm_tab[i].bs_npages;
                    if (vpage >= vpage_start && vpage < vpage_end) {
                        *store = i;
                        *pageth = vpage - vpage_start;
                        restore(ps);
                        return OK;
                    }
                }
                head = head->next;
            }
        }
    }
    restore(ps);
    return SYSERR;
}


