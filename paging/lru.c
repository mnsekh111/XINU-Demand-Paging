#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

void get_max_vpno(int frame, int *vpno_max) {
    int vpno_max_cnt = 0, j;
    STATWORD ps;
    disable(ps);
    for (j = NPROC - 1; j > 0; j--) {
        if (frm_tab[frame].fr_vpno[j] > vpno_max_cnt)
            vpno_max_cnt = frm_tab[frame].fr_vpno[j];
    }
    *vpno_max = vpno_max_cnt;
    restore(ps);
}

int LRU_nextframe() {
    int i, min_count = gtimecount, vpno_max = 0, frame_min = -1, vpno_max_cnt = 0;
    STATWORD ps;
    disable(ps);
    for (i = 0; i < NFRAMES; i++) {
        if ((min_count >= frm_tab[i].fr_loadtime) && (frm_tab[i].fr_status == FRM_MAPPED) && frm_tab[i].fr_type == FR_PAGE) {
            get_max_vpno(i, &vpno_max);
            if (frm_tab[i].fr_loadtime == min_count) {
                if (vpno_max > vpno_max_cnt) {
                    frame_min = i;
                    vpno_max_cnt = vpno_max;
                }
            } else {
                frame_min = i;
                vpno_max_cnt = vpno_max;
                min_count = frm_tab[i].fr_loadtime;
            }
        }
    }
    restore(ps);
    return frame_min;
}
