/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint() {
    STATWORD ps;
    disable(ps);
    unsigned long pfault_addr;
    int vpage, p, q;
    unsigned long pd_basereg;
    int store, pageth;
    int fr, fr1 = -1, i;
    pt_t *ptr_pt;
    pfault_addr = read_cr2();
    //kprintf("page fault address = 0x%x",pfault_addr);
    vpage = (pfault_addr & ~(NBPG - 1)) >> 12;
    pd_basereg = proctab[currpid].pdbr;
    if (bsm_lookup(currpid, pfault_addr, &store, &pageth) == SYSERR) {
        //kprintf("not a valid address\n");
        //kprintf("page fault address = 0x%x",pfault_addr);
        kill(currpid);
        //showerror and kill process and then call resched
    }

    //kprintf("Result of bsm_lookup:\nstore id = %d\n",store);
    //kprintf("pg_off = 0x%x\n",pageth);	
    pd_t *ptr_pd = (pd_t *) (proctab[currpid].pdbr);
    p = (pfault_addr & 0xFFC00000) >> 22;
    q = (pfault_addr & 0x003FF000) >> 12;
    //kprintf("p = %x\n",p);
    //kprintf("q = %x\n",q);
    ptr_pd = ptr_pd + p;
    if (ptr_pd->pd_pres == 0) {
        get_frm(&fr);
        //kprintf("%d",fr);
        frm_tab[fr].fr_pid = currpid; //not needed. It is done in get_frm
        frm_tab[fr].fr_type = FR_TBL;
        //kprintf(" frm_tab[fr].fr_type=%d", frm_tab[fr].fr_type);
        //  kprintf("pdbr = 0x%x\n",proctab[currpid].pdbr);
        fr = fr + FRAME0;
        add_page_dir(ptr_pd, fr, ((proctab[currpid].pdbr & ~(NBPG - 1)) >> 12) - FRAME0);
        //kprintf("PD frame for pid %d is : ((proctab[currpid].pdbr & ~NBPG)>>12)-FRAME0 = %d\n",currpid,((proctab[currpid].pdbr & ~(NBPG-1))>>12)-FRAME0);
    } else
        fr = ptr_pd->pd_base;
    // if(frm_tab[fr-FRAME0].fr_type == FR_TBL)
    // kprintf("fr for FR_TBL = %d\n",fr);
    ptr_pt = (pt_t *) (fr * NBPG);
    ptr_pt = ptr_pt + q;
    if (ptr_pt->pt_pres == 0) {
        for (i = 0; i < NFRAMES; i++) {
            if (frm_tab[i].fr_status == FRM_MAPPED) {
                bs_map_t * bs_temp = frm_tab[i].fr_bs_list;
                if (bs_temp != NULL) {
                    if (bs_temp->bs_status == BSM_MAPPED) {
                        if (frm_tab[i].fr_curr_page == pageth && frm_tab[i].fr_curr_bs == store) {
                            fr1 = i;
                            //kprintf("frame %d shared\n",fr1);
                            //update_frame_tohead_fifoqueue(i);
                        }
                    }
                }
            }
        }
        if (fr1 == -1) {
            gtimecount++;
            if (page_replace_policy == LRU) {
                for (i = 0; i < NFRAMES; i++)
                    check_update_timecount(i);
                get_frm(&fr1);
            } else {
                get_frm(&fr1);
                addentry_fifoqueue(fr1);
            }
            frm_tab[fr1].fr_type = FR_PAGE;
            //	kprintf("fr1 for FR_PAGE = %d\n",fr1);
            int *phy_page = (fr1 + FRAME0) * NBPG;
            read_bs(phy_page, store, pageth);
        }
        fr1 = fr1 + FRAME0;
        frm_tab[fr1 - FRAME0].fr_curr_bs = store;
        frm_tab[fr1 - FRAME0].fr_bs_list = &bsm_tab[store];
        frm_tab[fr1 - FRAME0].fr_vpno[currpid] = vpage;
        frm_tab[fr1 - FRAME0].fr_refcnt++;
        frm_tab[fr1 - FRAME0].fr_curr_page = pageth;
        add_page_tab(ptr_pt, fr1, (ptr_pd->pd_base) - FRAME0);
        //kprintf("fr1-FRAME0 = %d\n",fr1-FRAME0);
        //kprintf("frm_tab[fr1-FRAME0].fr_vpno[%d] =0x%x\n",currpid,frm_tab[fr1-FRAME0].fr_vpno[currpid]);
        //kprintf("(ptr_pd->pd_base)-FRAME0 = %d. This is for FR_TBL\n",(ptr_pd->pd_base)-FRAME0);
    }
    restore(ps);
    return OK;
}


