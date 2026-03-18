#pragma once
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
typedef struct QueueNode {
    struct QueueNode* next;
    struct QueueNode* prev;
    void*             data;
} QueueNode;

typedef struct Queue {
    QueueNode*   head;
    QueueNode*   tail;
    size_t       size;
    const size_t element_size;
} Queue;

static inline void debug_print_q(Queue* q) {
    QueueNode* cur = q->tail;
    printf("[");
    while (cur) {
        printf("%p->", cur->data);
        cur = cur->next;
        printf(", ");
    }
    printf("]\n");
    printf("Queue located @%p:\n", q);
    printf("Queue{\n"
           "    QueueNode*    front = %p;\n"
           "    QueueNode*     back = %p;\n"
           "    size_t         size = %zu;\n"
           "    size_t element_size = %zu;\n"
           "}\n",
           q->head, q->tail, q->size, q->element_size);
}
Queue pq_create(const size_t element_size);
void  pq_destroy(Queue* q);

// peeks from front of queue (FIFO)
void* pq_peek(Queue* q);
// pops from front of queue (FIFO)
void* pq_pop(Queue* q);
// pushes to back of queue
void pq_push(Queue* q, void* element);
void q_print(Queue* q, void (*print_function)(void*));
