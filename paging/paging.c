#include <kernel.h>
#include <paging.h>
#include <proc.h>

void init_page_dir(pd_t *d) {
    d->pd_pres = 1;
    d->pd_write = 1;
    d->pd_user = 0;
    d->pd_pwt = 0;
    d->pd_acc = 0;
    d->pd_global = 0;
    d->pd_avail = 0;
    d->pd_base = 0;
}

void init_page_tab(pt_t *t) {
    t->pt_pres = 1;
    t->pt_write = 1;
    t->pt_user = 0;
    t->pt_acc = 0;
    t->pt_dirty = 0;
    t->pt_global = 0;
    t->pt_avail = 0;
    t->pt_base = 0;
}


