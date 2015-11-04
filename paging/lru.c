#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int max_vpn(int fr) {
    int max = 0;
    int i;

    for (i = 1; i < NPROC; i++)
        if (frm_tab[fr].fr_vpno[i] > max)
            max = frm_tab[fr].fr_vpno[i];

    return max;

}

int next_frame() {
    int i;
    int min = gtimecount, max = 0;
    int max_vp = 0, lru_frame = -1;

    for (i = 0; i < NFRAMES; i++) {
        if (frm_tab[i].fr_status == FRM_MAPPED && frm_tab[i].fr_type == FR_PAGE) {
            
            if (min >= frm_tab[i].fr_loadtime) {
                max_vp = max_vpn(i);
                if (frm_tab[i].fr_loadtime == min) {
                    if (max_vp > max) {
                        lru_frame = i;
                        max = max_vp;
                    }
                } else {
                    lru_frame = i;
                    max = max_vp;
                    min = frm_tab[i].fr_loadtime;
                }
            }
        }
    }
    return lru_frame;
}
