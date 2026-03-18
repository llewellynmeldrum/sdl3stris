#include "queue.h"
#include "logger.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Queue q_create(const size_t element_size) {
    return (Queue){
        .size = 0,
        .element_size = element_size,
        .head = NULL,
        .tail = NULL,
    };
}
void q_print(Queue* q, void (*print_function)(void*)) {
    QueueNode* cur = q->tail;
    printf("[");
    while (cur) {
        print_function(cur->data);
        cur = cur->next;
        printf(", ");
    }
    printf("]\n");
}

void* q_peek(Queue* q) {
    return q->head;
}

void* q_pop(Queue* q) {
    if (q->size == 0 || !q->head) {
        LOGERR("Tried to pop from empty queue @%p", q);
        return NULL;
    }
    QueueNode* victim = q->head;
    if (q->size > 1) {
        QueueNode* new_head = q->head->prev;
        q->head = new_head;
        q->head->next = NULL;
    } else {  // size of 1 means we need to nuke both
        q->tail = NULL;
        q->head = NULL;
    }
    q->size--;
    void* res = victim->data;
    free(victim);
    return res;
}

void q_push(Queue* q, void* src) {
    QueueNode* newtail = calloc(1, sizeof(*newtail));
    if (!newtail) {
        LOGERR("Calloc returned null.");
    }
    newtail->data = src;
    QueueNode* oldtail = q->tail;
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

void q_destroy(Queue* q) {
    while (q->size > 0) {
        q_pop(q);
    }
}
