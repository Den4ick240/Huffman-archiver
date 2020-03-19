#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Heap.h"
#include "BinaryTree.h"

void printb(int b, FILE *jake)
{
	static unsigned char out = 0
		, i = 0;

	if (jake == NULL)
	{
		out = i = 0;
		return;
	}
	if (b == 2)
	{
		fwrite(&out, 1, 1, jake);
		return;
	}
	if (i > 7)
	{
		fwrite(&out, 1, 1, jake);
		i = 0;
		out = 0;
	}
	if (b == 1)
		out = out | (1 << i);
	i++;
}

void Count_chars(unsigned int char_cnt[256], FILE * finn, unsigned int part)
{
	unsigned char	x = 0;
	unsigned int	number = 0;

	fseek(finn, 0, SEEK_SET);
	while (1)
	{
		fread(&x, 1, 1, finn);
		if (feof(finn))
			break;
		char_cnt[x]++;
		number++;
		if (number % part == 0)
			printf("%u%% is counted\n", number / part);
	}
	return;
}

TreeNode *Add_in_heap(Heap * heap, unsigned int char_cnt[256])
{
	int			i;
	TreeNode	*n = NULL;

	for (i = 0; i < 256; i++)
	{
		if (char_cnt[i])
		{
			n = TreeNode_New(i, char_cnt[i]);
			heap_push(heap, n, n->f);
		}
	}
	return n;
}

TreeNode* Build_tree(TreeNode* head, Heap* heap)
{
	TreeNode *buff1, *buff2;

	while (heap->size > 1)
	{
		heappop(heap, buff1);
		heappop(heap, buff2);
		head = TreeNode_New(0, buff1->f + buff2->f);
		head->l = buff1;
		head->r = buff2;
		heap_push(heap, head, head->f);
	}
	return head;
}

void WriteTree(FILE *jake, TreeNode *head)
{
	if (head->l)
	{
		printb(0, jake);
		WriteTree(jake, head->l);
		WriteTree(jake, head->r);
	}
	else
	{
		int j;
		printb(1, jake);
		for (j = 0; j < 8; j++)
			printb((head->c >> j) & 1, jake);
	}
}

void WriteResult(FILE *finn, FILE *jake, char codes[256][MAX_CODE_SIZE], unsigned int total_number)
{
	unsigned int h
		, ready = 0
		, part = total_number / 100;

	if (total_number % 100 > 0)
		part++;
	if (part == 0)
		part = 1;
	
	fseek(finn, 0, SEEK_SET);
	while (1)
	{
		unsigned char x
			, l
			, j;

		fread(&x, 1, 1, finn);
		if (feof(finn))
			break;
		l = strlen(codes[x]);
		for (j = 0; j < l; j++)
		{
			printb(codes[x][j] - '0', jake);
		}

		ready++;
		if (ready % part == 0)
		{
			h = ready / part;
			printf("%u%% is coded\n", h);
		}
	}
	printb(2, jake);
	return;
}

void Encode(FILE *finn, FILE *jake) // MAIN
{
	Heap			*heap_ch = Heap_create();
	TreeNode		*tree_head = NULL;
	unsigned int	char_cnt[256] = { 0 };
	char			codes[256][MAX_CODE_SIZE];
	unsigned int	total_number;
	unsigned int	part;

	fseek(finn, 0, SEEK_END);
	total_number = ftell(finn);
	if (total_number == 0)
	{
		fwrite(&total_number, 4, 1, jake);
		printf("DONE!\n");
		return;
	}

	printb(0, NULL);				//initialization of the bit stream
	
	part = total_number / 100;
	if (total_number % 100 || part == 0)
		part++;

	Count_chars(char_cnt, finn, part);
	printf("Bytes has been counted.\nTotal number: %u\n", total_number);

	fwrite(&total_number, 4, 1, jake);
	tree_head = Add_in_heap(heap_ch, char_cnt);
	printf("Heap has been formed\n");

	tree_head = Build_tree(tree_head, heap_ch);
	printf("Tree has been built\n");

	printf("Tree writing...\n");
	WriteTree(jake, tree_head);
	printf("Tree has been writed\n");

	if (tree_head->l == NULL)					//In case all bytes are the same
	{
		printb(2, jake);
		printf("DONE!\n");
		return;
	}

	FillCodes(tree_head, codes);
	printf("Codes has been prepeared\n");

	WriteResult(finn, jake, codes, total_number);
	printb(2, jake);

	FillCodes(tree_head, NULL);
	free(heap_ch);

	printf("DONE!\n");

	return;
}