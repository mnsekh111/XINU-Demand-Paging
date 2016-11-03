#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
    if (npages < 1 || npages > 128 || bs_id < 0 || bs_id > MAX_ID) {
        return SYSERR;
    }
    STATWORD ps;
    disable(ps);
    if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
        if (bsm_tab[bs_id].bs_private == 1) {
            restore(ps);
            return SYSERR;
        }
        npages = bsm_tab[bs_id].bs_npages;
    } else {
        bsm_tab[bs_id].bs_status = BSM_MAPPED;
        bsm_tab[bs_id].bs_npages = npages;
    }
    restore(ps);
    return npages;
}


