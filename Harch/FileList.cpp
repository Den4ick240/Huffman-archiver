#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Checks.h"

typedef struct File_in_arch_s
{
	char size, *name;
	unsigned int file_size, start, end;
} File_in_arch;

File_in_arch *ScanFileList(FILE *arch, unsigned int *num_files, unsigned int additional)
{
	unsigned int i;
	File_in_arch *out;

	fseek(arch, -8, SEEK_END);
	fread(num_files, 4, 1, arch);
	fseek(arch, *num_files, SEEK_SET);
	fread(num_files, 4, 1, arch);

	out = (File_in_arch*)malloc(sizeof(File_in_arch) * (*num_files + additional));

	for (i = 0; i < *num_files; i++)
	{
		fread(&(out[i].size), 4, 1, arch);
		out[i].name = (char*)malloc(out[i].size + 1);
		fread(out[i].name, 1, out[i].size, arch);
		out[i].name[out[i].size] = 0;
		fread(&(out[i].file_size), 4, 1, arch);
		//fread(&(out[i].hash), 4, 1, arch);
		fread(&(out[i].start), 4, 1, arch);
		fread(&(out[i].end), 4, 1, arch);
	}

	return out;
}

void PrintFileList(File_in_arch *file_list, unsigned int num_files, FILE *arch)
{
	unsigned int i
		, cnt = 0
		, pos;

	pos = ftell(arch);
	fwrite(&num_files, 4, 1, arch);

	for (i = 0; i < num_files; i++)
	{
		if (file_list[i].size == 0)
			continue;
		cnt++;
		fwrite(&(file_list[i].size), 4, 1, arch);
		fwrite(file_list[i].name, 1, file_list[i].size, arch);
		fwrite(&(file_list[i].file_size), 4, 1, arch);
		//fwrite(&(file_list[i].hash), 4, 1, arch);
		fwrite(&(file_list[i].start), 4, 1, arch);
		fwrite(&(file_list[i].end), 4, 1, arch);
	}

	if (cnt != i)
	{
		i = ftell(arch);
		fseek(arch, pos, SEEK_SET);
		fwrite(&cnt, 4, 1, arch);
		fseek(arch, i, SEEK_SET);
	}

	i = ftell(arch);
	fseek(arch, 0, SEEK_SET);
	printf("Counting hash-code...\n");
	i = Adler32(arch, i);
	fseek(arch, 0, SEEK_CUR);
	fwrite(&i, 4, 1, arch);
}

unsigned int FindFileInList(File_in_arch *file_list, unsigned int num, char *name, unsigned int *idx)
{
	unsigned int i;
	for (i = 0; i < num; i++)
		if (!strcmp(name, file_list[i].name))
		{
			if (idx != NULL)
				*idx = i;
			return 1;
		}
	return 0;
}

void FreeFileList(File_in_arch *file_list, unsigned int num)
{
	unsigned int i;
	for (i = 0; i < num; i++)
		free(file_list[i].name);
	free(file_list);
}