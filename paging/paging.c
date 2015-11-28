#include <kernel.h>
#include <paging.h>
#include <proc.h>

void init_pd_entry(pd_t * entry) {
	entry->pd_pres = 1;
	entry->pd_write = 1;
	entry->pd_user = 0;
	entry->pd_pwt = 0;
	entry->pd_acc = 0;
	entry->pd_mbz = 0;
	entry->pd_fmb = 0;
	entry->pd_global = 0;
	entry->pd_avail = 0;
	entry->pd_base = 0;
}

void init_pt_entry(pt_t * entry) {
	entry->pt_pres = 1;
	entry->pt_write = 1;
	entry->pt_user = 0;
	entry->pt_pwt = 0;
	entry->pt_pcd = 0;
	entry->pt_acc = 0;
	entry->pt_dirty = 0;
	entry->pt_mbz = 0;
	entry->pt_global = 0;
	entry->pt_avail = 0;
	entry->pt_base = 0;
}

void init_global_pg_tab() {


	kprintf("Inside init global pt");
	int new_frame_pd, new_frame_pt;
	int i, j;
	//Get a new frame for page directory
	get_frm(&new_frame_pd);
	frm_tab[new_frame_pd].fr_type = FR_DIR;
	frm_tab[new_frame_pd].fr_pid = NULLPROC;


	pd_t * dir_entry; // For tables
	pt_t * tab_entry; // For pages

	dir_entry = (pd_t*) ((new_frame_pd + FRAME0) * NBPG);

	for (i = 0; i < NGPT; i++) {

		get_frm(&new_frame_pt);
		kprintf("New frame %d",new_frame_pt);
		frm_tab[new_frame_pt].fr_type = FR_TBL;
		frm_tab[new_frame_pt].fr_pid = NULLPROC;


		tab_entry = (pt_t*) ((new_frame_pt + FRAME0) * NBPG);
		init_pd_entry(dir_entry);
		dir_entry->pd_base = new_frame_pt + FRAME0;

		//Ref count incremented when entry is added to this frame
		frm_tab[new_frame_pd].fr_refcnt++;


		for (j = 0; j < NBPG / 4; j++) { // Size of each entry is 32 bit or 4 bytes
			init_pt_entry(tab_entry);

			tab_entry->pt_base = (i*NBPG / 4)+j;
			frm_tab[new_frame_pt].fr_refcnt++;

			tab_entry++;
		}

		dir_entry++;
	}

}
