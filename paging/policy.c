/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;

/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy) {
    STATWORD ps;
    disable(ps);
    if (policy == FIFO) {
        page_replace_policy = FIFO;
        init_queue();
    } else if (policy == LRU)
        page_replace_policy = LRU;

    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy() {
    return page_replace_policy;
}
