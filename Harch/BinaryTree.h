#pragma once
#define MAX_CODE_SIZE 256
typedef struct TreeNode_s
{
	unsigned char c;
	unsigned int f;
	struct TreeNode_s *l, *r;
} TreeNode;

TreeNode * TreeNode_New(unsigned char c, unsigned int f);
void FillCodes(TreeNode *head, char codes[256][MAX_CODE_SIZE]);