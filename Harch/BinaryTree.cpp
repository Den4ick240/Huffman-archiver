#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CODE_SIZE 256

typedef struct TreeNode_s
{
	unsigned char c;
	unsigned int f;
	struct TreeNode_s *l, *r;
} TreeNode;

TreeNode * TreeNode_New(unsigned char c, unsigned int f)
{
	TreeNode * out = (TreeNode*)malloc(sizeof(TreeNode));
	out->c = c;
	out->f = f;
	out->l = out->r = NULL;
	return out;
}

void CleanTree(TreeNode* tree)
{
	if (tree->l != NULL)
	{
		CleanTree(tree->l);
		CleanTree(tree->r);
	}
	free(tree);
}

void FillCodes(TreeNode *head, char codes[256][MAX_CODE_SIZE])
{
	static char code[MAX_CODE_SIZE]
		, n = 0;

	if (codes == NULL)
	{
		n = 0;
		CleanTree(head);
		return;
	}

	if (head->l)
	{
		code[n++] = '1';
		FillCodes(head->r, codes);
		code[n++] = '0';
		FillCodes(head->l, codes);
		n--;
	}
	else
	{
		code[n] = 0;
		memcpy(codes[head->c], code, MAX_CODE_SIZE);
		n--;
		code[n] = 0;
	}
}