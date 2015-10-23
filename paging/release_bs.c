#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

	/* release the backing store with ID bs_id */
	if (bs_id < 0 || bs_id > MAX_ID)
		return SYSERR;

	STATWORD ps;
	disable(ps);

	//No process is being mapped to the backing store
	if (bsm_tab[bs_id].bs_head == NULL) {
		free_bsm(bs_id);
		restore(ps);
		return OK;
	}

	//If some process is being mapped to this bs
	//then it's illegal to release this bs;

	restore(ps);
	return SYSERR;

}

