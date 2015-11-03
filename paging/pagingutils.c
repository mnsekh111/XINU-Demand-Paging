#include <kernel.h>
#include <paging.h>
#include <proc.h>

void add_page_dir(pd_t *ptr, unsigned int fr, unsigned int pd) {
    STATWORD ps;
    disable(ps);
    init_page_dir(ptr);
    ptr->pd_base = fr & 0xFFFFF;
    frm_tab[pd].fr_refcnt++;
    restore(ps);
}

void add_page_tab(pt_t *ptr, unsigned int fr, unsigned int pt) {
    STATWORD ps;
    disable(ps);
    init_page_tab(ptr);
    ptr->pt_base = fr & 0xFFFFF;
    frm_tab[pt].fr_refcnt++;
    restore(ps);
}

void reset_page_tab(int vp) {
    int X, Y;
    STATWORD ps;
    disable(ps);
    int * vaddadd;
    *vaddadd = vp*NBPG;
    virt_addr_t * vadd = (virt_addr_t*) vaddadd;

    X = vadd->pd_offset;
    Y = vadd->pt_offset;
    pd_t *new_dir = ((pd_t*) (proctab[currpid].pdbr)) + X;
    pt_t *new_tab = ((pt_t*) ((new_dir->pd_base) * NBPG)) + Y;
    init_page_tab(new_tab);
    new_tab->pt_pres = 0;
    
    frm_tab[(new_dir->pd_base) - FRAME0].fr_refcnt--;
    restore(ps);

}
