#include <stdio.h>
#include "FileList.h"
#define MOD_VAL 65521
#define ARR_SIZE 1000000

typedef union {
	unsigned int i;
	struct
	{
		unsigned short a, b;
	}ch;
} Adler32_union;

unsigned int Adler32(FILE *finn, unsigned int num)
{
	unsigned int	i;
	unsigned int	j;
	unsigned int	n;
	unsigned char	*x = (unsigned char*)malloc(ARR_SIZE);
	Adler32_union	un;
	un.ch.a = 1;
	un.ch.b = 0;
	n = num / ARR_SIZE;

	for (i = 0; i < n; i++)
	{
		fread(x, 1, ARR_SIZE, finn);
		for (j = 0; j < ARR_SIZE; j++)
		{
			un.ch.a = (un.ch.a + x[j]) % MOD_VAL;
			un.ch.b = (un.ch.b + un.ch.a) % MOD_VAL;
		}
	}
	n = num % ARR_SIZE;
	fread(x, 1, n, finn);
	for (i = 0; i < n; i++)
	{
		un.ch.a = (un.ch.a + x[i]) % MOD_VAL;
		un.ch.b = (un.ch.b + un.ch.a) % MOD_VAL;
	}
	free(x);
	return un.i;
}

int CheckIntegrity(char *name)
{
	FILE			*arch = fopen(name, "rb");
	unsigned int	num;
	unsigned int	hash;

	fseek(arch, 0, SEEK_END);
	num = ftell(arch);
	if (num < 8)
		return 1;
	num -= 4;
	fseek(arch, 0, SEEK_SET);
	num = Adler32(arch, num);
	fread(&hash, 4, 1, arch);
	if (num != hash)
	{
		printf("Archive is corrupted!\n");
		return 1;
	}
	printf("Archive seems to be uncorrupted.\n");
	fclose(arch);
	return 0;
}

int CheckExt(char *a)
{
	while (*a != '.')
		if (*(a++) == 0)
			return 1;
	if (*(++a) == 0)
		return 1;
	else if (*(a++) == 'a')
		if (*(a) == 0)
			return 1;
		else if (*(a++) == 'r')
			if (*(a) == 0)
				return 1;
			else if (*(a++) == 'c')
				if (*(a) == 0)
					return 1;
				else if (*(a++) == 'h')
					if (*a == 0)
						return 0;
					else
						return 1;
	return 1;
}

unsigned int CheckFilesExistence(int argc, char* argv[])
{
	FILE *finn;
	int i;
	for (i = 3; i < argc; i++)
	{
		finn = fopen(argv[i], "rb");
		if (finn == NULL)
			return i;
		else
			fclose(finn);
	}
	return 0;
}