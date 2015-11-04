/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm(unsigned int frame) {
    STATWORD ps;
    int i;
    disable(ps);
    frm_tab[frame].fr_status = FRM_UNMAPPED;
    frm_tab[frame].fr_pid = -1;
    frm_tab[frame].fr_curr_bs = -1;
    frm_tab[frame].fr_bs_list = NULL;
    frm_tab[frame].fr_refcnt = 0;
    frm_tab[frame].fr_dirty = 0;
    frm_tab[frame].fr_curr_page = -1;
    frm_tab[frame].fr_type = -1;
    frm_tab[frame].fr_loadtime = 0;
    for (i = 0; i < NPROC; i++) {
        frm_tab[frame].fr_vpno[i] = 0;
    }

    /* Not sure if below routine is needed. Just to be safe */
    pt_t *ptr4 = (pt_t *) ((frame + FRAME0) * NBPG);
    for (i = 0; i < 1024; i++, ptr4++) {
        ptr4->pt_pres = 0;
        ptr4->pt_write = 0;
        ptr4->pt_user = 0;
        ptr4->pt_pwt = 0;
        ptr4->pt_pcd = 0;
        ptr4->pt_acc = 0;
        ptr4->pt_dirty = 0;
        ptr4->pt_mbz = 0;
        ptr4->pt_global = 0;
        ptr4->pt_avail = 0;
        ptr4->pt_base = 0;
    }
    restore(ps);
    //kprintf("To be implemented!\n");
    return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail) {
    int i, j;
    STATWORD ps;
    disable(ps);
    for (i = 0; i < NFRAMES; i++) {
        if (frm_tab[i].fr_status == FRM_UNMAPPED) {
            init_frm(i);
            *avail = i;
            frm_tab[i].fr_status = FRM_MAPPED;
            //kprintf("get_frm returned %d\n",*avail);
            break;
        }
    }
    if (i == NFRAMES) {
        //  kprintf("Ran out of free frames\n");
        //indicates that we ran out of free frames. Implement page replacement policy and return a new frame as per the page replacement policy
        if (page_replace_policy == LRU) {
            *avail = next_frame();
            //kprintf("get_frm returned %d\n",*avail);
        } else {
            *avail = getframe_fifoqueue();
        }
        if (check_update_bs(*avail, frm_tab[*avail].fr_curr_bs, currpid) == OK) {
            frm_tab[*avail].fr_refcnt--;
            for (j = NPROC - 1; j > 0; j--) {
                if (frm_tab[*avail].fr_vpno[j] > 0 && frm_tab[*avail].fr_type == FR_PAGE) {
                    //kprintf("fr_vpno[%d] = 0x%x\n",j,frm_tab[*avail].fr_vpno[j]);
                    free_frm(*avail, j);
                }
            }

            //kprintf("ran out of free frames\n");
        }
        frm_tab[*avail].fr_status = FRM_MAPPED;
    }
    //kprintf("get_frm returned %d\n",*avail);
    frm_tab[*avail].fr_pid = currpid;
    restore(ps);
    //kprintf("To be implemented!\n");
    return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i, int pid) {
    STATWORD ps;
    disable(ps);
    int k = NFRAMES;
    if (frm_tab[i].fr_refcnt <= 0) {
        if (frm_tab[i].fr_type == FR_PAGE) {
            reset_page_tab(frm_tab[i].fr_vpno[pid]);
            init_frm(i);
        } else if (frm_tab[i].fr_type == FR_TBL) {
            pd_t *ptr = (pd_t*) (proctab[pid].pdbr);
            while (k > 0) {
                if (ptr->pd_base == (i + FRAME0)) {

                    init_page_dir(ptr);
                    ptr->pd_pres = 0;
                    break;
                }
                ptr++;
                k--;
            }
            init_frm(i);

        }

    } else {
        if (frm_tab[i].fr_type == FR_PAGE) {

            reset_page_tab(frm_tab[i].fr_vpno[pid]);
            frm_tab[i].fr_vpno[pid] = 0;
        }
    }

    restore(ps);
    return OK;
}







