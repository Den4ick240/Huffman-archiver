#pragma once
#define heappop(heap,x) heap_pop(heap, (void**)&x)

typedef unsigned int Heap_key_type;
typedef void* Heap_val_type;

typedef struct Heap_node_s
{
	Heap_val_type val;
	Heap_key_type key;
} Heap_node;

typedef struct Heap_s
{
	int size;
	Heap_node *arr;
} Heap;

Heap_key_type heap_pop(Heap * heap, Heap_val_type * data);
void heap_push(Heap * heap, Heap_val_type val, Heap_key_type key);
Heap * Heap_create();