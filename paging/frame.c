/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

void reset_frm(int frame) {
	frm_tab[frame].fr_status = FRM_UNMAPPED;
	frm_tab[frame].fr_pid = -1;
	frm_tab[frame].fr_vpno = -1;
	frm_tab[frame].fr_refcnt = 0;
	frm_tab[frame].fr_type = -1;
	frm_tab[frame].fr_dirty = 0;
	frm_tab[frame].fr_loadtime = 0;
}

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */SYSCALL init_frm() {
	STATWORD ps;
	disable(ps);

	int i;
	for (i = 0; i < NFRAMES; i++) {
		reset_frm(i);
	}

	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */SYSCALL get_frm(int* avail) {

	STATWORD ps;
	disable(ps);
	int i;
	int found = 0;

	for (i = 0; i < NFRAMES; i++) {
		if (frm_tab[i].fr_status == FRM_UNMAPPED) {
			*avail = i;
			reset_frm(*avail);
			frm_tab[*avail].fr_status = FRM_MAPPED;
			found = 1;
			break;
		}
	}

	if (found == 0) {
		restore(ps);
		return SYSERR;
	}

	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */SYSCALL free_frm(int i) {

	kprintf("To be implemented!\n");
	return OK;
}

