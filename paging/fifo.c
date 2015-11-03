#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

struct fifoqueue *head;
struct fifoqueue *tail;

void init_fifoqueue() {
    STATWORD ps;
    disable(ps);
    head = NULL;
    tail = NULL;
    restore(ps);
}

void addentry_fifoqueue(int frame) {
    STATWORD ps;
    disable(ps);
    struct fifoqueue *temp = (struct fifoqueue*) getmem(sizeof (struct fifoqueue));
    temp->framenumber = frame;
    temp->nextf = NULL;
    if (head == NULL && tail == NULL) {
        head = temp;
        tail = temp;
    } else {
        tail->nextf = temp;
        tail = temp;
    }

    restore(ps);
}

int getframe_fifoqueue() {
    STATWORD ps;
    disable(ps);
    if (head != NULL) {
        int frame = head->framenumber;
        struct fifoqueue *tem = head;
        head = head->nextf;
        freemem(tem, sizeof (struct fifoqueue));
        restore(ps);
        return frame;
    } else {
        restore(ps);
        return SYSERR;
    }
}

void update_frame_tohead_fifoqueue(int frame) {
    STATWORD ps;
    disable(ps);
    struct fifoqueue *test = head;
    if (head->framenumber == frame) {
        head = head->nextf;
        tail->nextf = test;
        tail = test;
        test->nextf = NULL;
        restore(ps);
        return;
    }
    while (test->nextf != NULL) {
        if (test->nextf->framenumber == frame) {
            tail->nextf = test->nextf;
            tail = tail->nextf;
            test->nextf = test->nextf->nextf;
            tail->nextf = NULL;
            restore(ps);
            return;
        }
        test = test->nextf;
    }
    restore(ps);
}
