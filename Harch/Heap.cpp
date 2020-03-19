#include <stdio.h>
#include <stdlib.h>

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
	Heap_node arr[257];
} Heap;

void sift_up(Heap * heap, int idx)
{
	int anc = idx / 2;

	if (idx > 1 && heap->arr[idx].key < heap->arr[anc].key)
	{
		Heap_node x = heap->arr[idx];
		heap->arr[idx] = heap->arr[anc];
		heap->arr[anc] = x;
		sift_up(heap, anc);
	}
}

void sift_down(Heap * heap, int idx)
{
	int l = idx * 2
		, r = idx * 2 + 1
		, min = idx;

	if (l <= heap->size)
		if (r <= heap->size && heap->arr[r].key < heap->arr[l].key)
			min = r;
		else
			min = l;

	if (heap->arr[min].key < heap->arr[idx].key)
	{
		Heap_node x = heap->arr[min];
		heap->arr[min] = heap->arr[idx];
		heap->arr[idx] = x;
		sift_down(heap, min);
	}

	return;
}

Heap_key_type heap_pop(Heap * heap, Heap_val_type * data)
{
	Heap_key_type out = heap->arr[1].key;
	*data = heap->arr[1].val;
	heap->arr[1] = heap->arr[(heap->size)--];
	sift_down(heap, 1);
	return out;
}

void heap_push(Heap * heap, Heap_val_type val, Heap_key_type key)
{
	heap->arr[++(heap->size)].val = val;
	heap->arr[heap->size].key = key;
	sift_up(heap, heap->size);
}

Heap * Heap_create()
{
	Heap * out = (Heap*)malloc(sizeof(Heap));
	out->size = 0;
	return out;
}