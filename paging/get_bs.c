#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*
 * This call requests a new backing store with id
 * store of size npages (in pages, not bytes).
 * If the new backing store cannot be created, or a
 * backing store with this ID already exists, the size
 * of the new or existing backing store is returned.
 * This size is in pages. If a size of 0 is requested,
 * or an error is encountered, SYSERR is returned.
 */SYSCALL get_bs(bsd_t bs_id, unsigned int npages) {

	/* requests a new mapping of npages with ID map_id */

	if (bs_id < 0 || bs_id > MAX_ID || npages < 1 || npages > MAX_BS_PG)
		return SYSERR;

	STATWORD ps;
	disable(ps);

	if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
		if (bsm_tab[bs_id].bs_private == 1) {
			restore(ps);
			return SYSERR;
		}
	} else {
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_npages = npages;
	}

	restore(ps);
	return bsm_tab[bs_id].bs_npages;

}

