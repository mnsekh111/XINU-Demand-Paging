/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */SYSCALL init_bsm() {

	STATWORD ps;
	disable(ps);
	int i;

	for (i = 0; i < NBS; i++) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_head = NULL;
		bsm_tab[i].bs_npages = -1;
		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].bs_private = 0;
	}

	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */SYSCALL get_bsm(int* avail) {
	STATWORD ps;
	disable(ps);
	int i;
	for (i = 0; i < NBS; i++) {
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
 */SYSCALL free_bsm(int i) {
	STATWORD ps;
	disable(ps);
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	bsm_tab[i].bs_head = NULL;
	bsm_tab[i].bs_npages = -1;
	bsm_tab[i].bs_private = 0;
	restore(ps);
	return OK;

}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {
}

/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */SYSCALL bsm_map(int pid, int vpno, int source, int npages) {
}

/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */SYSCALL bsm_unmap(int pid, int vpno, int flag) {
}

