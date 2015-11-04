#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

    /* release the backing store with ID bs_id */
    if (bs_id < 0 || bs_id > 15)
        return SYSERR;
    STATWORD ps;
    disable(ps);
    struct bs_proc_map_t *temp;
    temp = bsm_tab[bs_id].mapping;
    if (temp == NULL) {
        bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
        bsm_tab[bs_id].bs_npages = -1;
    }
    restore(ps);
    return OK;

}

