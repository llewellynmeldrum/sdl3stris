#pragma once
#include "piecedata.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#define PQ_CAPACITY 5
typedef struct QueueNode {
    struct QueueNode* next;
    struct QueueNode* prev;
    PieceType         type;
} PieceQueueNode;

typedef struct PieceQueue {
    PieceQueueNode* head;
    PieceQueueNode* tail;
    size_t          size;
} PieceQueue;

PieceQueue pq_create(void);
PieceType  pq_peek(PieceQueue* q);
PieceType  pq_pop(PieceQueue* q);
void       pq_push(PieceQueue* q, PieceType src);
void       pq_destroy(PieceQueue* q);
