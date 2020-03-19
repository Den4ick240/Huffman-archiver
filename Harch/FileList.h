#pragma once
#include <stdio.h>
#include <stdlib.h>
typedef struct File_in_arch_s
{
	char size, *name;
	unsigned int file_size, start, end;
} File_in_arch;
File_in_arch *ScanFileList(FILE *arch, unsigned int *num_files, unsigned int additional);
void PrintFileList(File_in_arch *file_list, unsigned int num_files, FILE *arch);
unsigned int FindFileInList(File_in_arch *file_list, unsigned int num, char *name, unsigned int *idx);
void FreeFileList(File_in_arch *file_list, unsigned int num);