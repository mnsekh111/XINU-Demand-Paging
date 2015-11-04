#include <kernel.h>
#include <proc.h>
#include <paging.h>

int check_update_bs(int frameno, int store, int pid) {
    STATWORD ps;
    disable(ps);
    if ((frm_tab[frameno].fr_status == FRM_MAPPED) && (frm_tab[frameno].fr_type == FR_PAGE)) {
        int vpno, fr_curr_page, X, Y, pageth;
        vpno = frm_tab[frameno].fr_vpno[pid];
        fr_curr_page = frm_tab[frameno].fr_curr_page;

        X = ((vpno * NBPG)&0xFFC00000) >> 22;
        Y = ((vpno * NBPG)&0x003FF000) >> 12;

        if (bsm_lookup(pid, vpno * NBPG, &store, &pageth) == SYSERR) {
            restore(ps);
            return SYSERR;
        }

        pd_t *pd1 = ((pd_t*) (proctab[pid].pdbr)) + X;
        pt_t *pt1 = ((pt_t*) ((pd1->pd_base) * NBPG)) + Y;

        if (pt1->pt_dirty == 1) {
            write_bs((frameno + FRAME0) * NBPG, store, pageth);
            pt1->pt_dirty = 0;
        }
        restore(ps);
        return OK;
    }
    restore(ps);
    return 0;

}

int remove_frames_for_xmunmap(int store) {
    STATWORD ps;
    disable(ps);
    int j;
    struct track_PT *temp = NULL;
    struct track_PT *track_PT_head = NULL;
    for (j = NFRAMES - 1; j >= 0; j--) {
        if (frm_tab[j].fr_curr_bs == store) {
            if (check_update_bs(j, store, currpid) == SYSERR) {

                restore(ps);
                return SYSERR;
            }
            frm_tab[j].fr_refcnt--;
            free_frm(j, currpid);
        }
        if (frm_tab[j].fr_type == FR_TBL && frm_tab[j].fr_pid == currpid) {
            temp = (struct track_PT *) getmem(sizeof (struct track_PT));
            temp->framenumber = j;
            temp->nextPT = track_PT_head;
            track_PT_head = temp;
        }
    }
    while (track_PT_head != NULL) {
        free_frm(track_PT_head->framenumber, currpid);
        track_PT_head = track_PT_head->nextPT;
    }
    restore(ps);
    return OK;
}

void remove_frames_for_kill(int pid) {
    int j;
    struct track_PT *temp = NULL;
    struct track_PT *track_PT_head = NULL;
    STATWORD ps;
    disable(ps);
    for (j = 5; j < NFRAMES; j++) {
        if (frm_tab[j].fr_vpno[pid] != 0) {
            if (check_update_bs(j, frm_tab[j].fr_curr_bs, pid) == OK) {
                frm_tab[j].fr_refcnt--;
                free_frm(j, pid);
            }
        }
        if (frm_tab[j].fr_type == FR_TBL && frm_tab[j].fr_pid == pid) {
            temp = (struct track_PT *) getmem(sizeof (struct track_PT));
            temp->framenumber = j;
            temp->nextPT = track_PT_head;
            track_PT_head = temp;
        }


    }
    while (track_PT_head != NULL) {

        free_frm(track_PT_head->framenumber, pid);
        track_PT_head = track_PT_head->nextPT;
    }

    int pd_frame = ((proctab[pid].pdbr) / NBPG) - FRAME0;

    frm_tab[pd_frame].fr_status = FRM_UNMAPPED;

    restore(ps);

}

void check_update_timecount(int frameno) {
    int i;
    STATWORD ps;
    disable(ps);
    if ((frm_tab[frameno].fr_status == FRM_MAPPED) && (frm_tab[frameno].fr_type == FR_PAGE)) {
        int vpno, X, Y, pageth;
        for (i = NPROC - 1; i > 0; i--) {
            if (frm_tab[frameno].fr_vpno[i] > 0) {
                vpno = frm_tab[frameno].fr_vpno[i];
                X = ((vpno * NBPG)&0xFFC00000) >> 22;
                Y = ((vpno * NBPG)&0x003FF000) >> 12;

                pd_t *pd1 = ((pd_t*) (proctab[currpid].pdbr)) + X;
                pt_t *pt1 = ((pt_t*) ((pd1->pd_base) * NBPG)) + Y;

                if (pt1->pt_acc == 1) {
                    frm_tab[frameno].fr_loadtime = gtimecount;
                    pt1->pt_acc = 0;
                }
            }
        }
    }
    restore(ps);
}