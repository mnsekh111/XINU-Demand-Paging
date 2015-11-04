#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

struct fifoqueue *head;
struct fifoqueue *tail;

void init_queue() {
    STATWORD ps;
    disable(ps);
    head = NULL;
    tail = NULL;
    restore(ps);
}

void push_queue(int frame) {
    STATWORD ps;
    disable(ps);
    struct fifoqueue *temp = (struct fifoqueue*) getmem(sizeof (struct fifoqueue));
    temp->fn = frame;
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
        int frame = head->fn;
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

