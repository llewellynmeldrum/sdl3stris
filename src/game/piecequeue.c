#include "game/piecequeue.h"
#include "logger.h"
#include <stdlib.h>

PieceQueue pq_create() {
    return (PieceQueue){
        .size = 0,
        .head = NULL,
        .tail = NULL,
    };
}
PieceType pq_peek(PieceQueue* q) {
    return q->head->type;
}

PieceType pq_pop(PieceQueue* q) {
    if (q->size == 0 || !q->head) {
        LOGERR("Tried to pop from empty queue @%p", q);
        return -1;
    }
    PieceQueueNode* victim = q->head;
    if (q->size > 1) {
        PieceQueueNode* new_head = q->head->prev;
        q->head = new_head;
        q->head->next = NULL;
    } else {  // size of 1 means we need to nuke both
        q->tail = NULL;
        q->head = NULL;
    }
    q->size--;
    PieceType res = victim->type;
    free(victim);
    return res;
}

void pq_push(PieceQueue* q, PieceType src) {
    PieceQueueNode* newtail = calloc(1, sizeof(*newtail));
    if (!newtail) {
        LOGERR("Calloc returned null.");
    }
    newtail->type = src;
    PieceQueueNode* oldtail = q->tail;
    q->tail = newtail;
    if (oldtail) {
        newtail->next = oldtail;
        oldtail->prev = newtail;
    }
    if (q->size == 0) {
        q->head = q->tail;
    }
    q->size++;
}

void pq_destroy(PieceQueue* q) {
    while (q->size > 0) {
        pq_pop(q);
    }
}
