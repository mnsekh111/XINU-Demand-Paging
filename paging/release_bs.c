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

    if (bsm_tab[bs_id].mapping == NULL) {
        free_bsm(bs_id);
        restore(ps);
        return OK;
    }
    restore(ps);
    return SYSERR;

}

