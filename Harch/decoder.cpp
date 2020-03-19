#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinaryTree.h"

char scanb(FILE *finn)			//The function scans bytes
{								//and returns one bit per request
									
	static unsigned char 
		in = 0					//Current byte from input
		, i = 0;				//Current bit position in byte

	if (finn == NULL)			//If argument is NULL then reset func
	{
		in = i = 0;				
		return 0;
	}
	if (i > 7 || i == 0)		//If byte has ended or it is the first call then read new one
	{
		i = 0;
		fread(&in, 1, 1, finn);
	}
	return (in >> i++) & 1;		//Return the bit number i and increace
}								//number of bit for the next request

TreeNode *BuildTree(FILE *finn)
{
	char x;
	TreeNode *tree;
	x = scanb(finn);				//Getting a bit
	if (x)							//If the bit is one tnen
	{								//create a leaf with a char
		int i;
		char b = 0;
		for (i = 0; i < 8; i++)
			b |= (scanb(finn) << i);//Reading the char in bits
		tree = TreeNode_New(b, 0);	
		tree->l = tree->r = NULL;	//The leaf with a char have no children 
	} 
	else							//If the bit is zero then 
	{								//create a node and
		tree = TreeNode_New(0, 0);	//continue building the tree recursively
		tree->l = BuildTree(finn);
		tree->r = BuildTree(finn);
	}
	return tree;					
}

void Decode(FILE *finn, FILE *jake)		//Main function for decoding
{
	TreeNode *tree_head = NULL
		, *curr;
	char b;
	unsigned int total_number
		, curr_number = 0
		, part;

	scanb(NULL);						//Initialization of scanb in case it was called later
	fread(&total_number, 4, 1, finn);	//First four bits contain the size of the source file 
	if (total_number == 0)
	{
		printf("DONE!\n");
		return;
	}

	part = total_number / 100;			//1% of the file size. This var is needed to display the progress
	if (total_number % 100 > 0 || part == 0)
		part++;

	tree_head = curr = BuildTree(finn);	
	printf("Tree has been built\n");

	if (tree_head->l == NULL)			//If the file consisted of identical bytes
	{									//then the root of tree is a leaf
		while (total_number--)			//therefore, we just write right amount of 
			fwrite(&(tree_head->c), 1, 1, jake);//identical bytes
		scanb(NULL);					//Reset function in case the function will is reused
		printf("DONE!\n");
		return; 
	}	
	while (curr_number < total_number)	
	{
		if (curr->l == NULL)				//If program reach the leaf it writes right byte
		{
			fwrite(&(curr->c), 1, 1, jake);
			curr = tree_head;				//Return to the root
			curr_number++;					//Increace amount of writed bytes
			
			if (curr_number % part == 0)	//Displaying progress
			{
				unsigned int h = curr_number / part;
				printf("%u%% is decoded\n", h);
			}
		}
		b = scanb(finn);		//Get a bit
		if (feof(finn))
			break;
		
		if (b)					//If the bit is one then go to right child
			curr = curr->r;
		else					//else go to left
			curr = curr->l;
	}
	printf("DONE!\n");
}