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
    
    int vp, p, q;
    int i;
    
    unsigned long fault_addr;
    
    int store, pageth;
    int new_frame, m_frame = -1;
    
   
    pt_t *ptr_pt;
    fault_addr = read_cr2();
    vp = (fault_addr & ~(NBPG - 1)) >> 12;
    
    //If  process tries to access bs that it has not got using getbs.
    if (bsm_lookup(currpid, fault_addr, &store, &pageth) == SYSERR) {
        kill(currpid);
    }


    pd_t *ptr_pd = (pd_t *) (proctab[currpid].pdbr);
    p = (fault_addr & 0xFFC00000) >> 22;
    q = (fault_addr & 0x003FF000) >> 12;

    ptr_pd = ptr_pd + p;
    if (ptr_pd->pd_pres == 0) {
        get_frm(&new_frame);

        frm_tab[new_frame].fr_pid = currpid; 
        frm_tab[new_frame].fr_type = FR_TBL;

        new_frame = new_frame + FRAME0;
        add_page_dir(ptr_pd, new_frame, ((proctab[currpid].pdbr & ~(NBPG - 1)) >> 12) - FRAME0);
       
    } else
        new_frame = ptr_pd->pd_base;

    
    
    ptr_pt = (pt_t *) (new_frame * NBPG);
    ptr_pt = ptr_pt + q;
    if (ptr_pt->pt_pres == 0) {
        for (i = 0; i < NFRAMES; i++) {
            if (frm_tab[i].fr_status == FRM_MAPPED) {
                bs_map_t * bs_temp = frm_tab[i].fr_bs_list;
                if (bs_temp != NULL) {
                    if (bs_temp->bs_status == BSM_MAPPED) {
                        if (frm_tab[i].fr_curr_page == pageth && frm_tab[i].fr_curr_bs == store) {
                            m_frame = i;
                        }
                    }
                }
            }
        }
        if (m_frame == -1) {
            gtimecount++;
            if (page_replace_policy == LRU) {
                for (i = 0; i < NFRAMES; i++)
                    update_age(i);
                get_frm(&m_frame);
            } else {
                get_frm(&m_frame);
                push_queue(m_frame);
            }
            frm_tab[m_frame].fr_type = FR_PAGE;
            int *phy_page = (m_frame + FRAME0) * NBPG;
            read_bs(phy_page, store, pageth);
        }
        m_frame = m_frame + FRAME0;
        frm_tab[m_frame - FRAME0].fr_curr_bs = store;
        frm_tab[m_frame - FRAME0].fr_bs_list = &bsm_tab[store];
        frm_tab[m_frame - FRAME0].fr_vpno[currpid] = vp;
        frm_tab[m_frame - FRAME0].fr_refcnt++;
        frm_tab[m_frame - FRAME0].fr_curr_page = pageth;
        add_page_tab(ptr_pt, m_frame, (ptr_pd->pd_base) - FRAME0);
    }
    restore(ps);
    return OK;
}


