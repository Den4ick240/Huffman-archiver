#pragma once
int CheckExt(char *a);
int CheckIntegrity(char *name);
unsigned int CheckFilesExistence(int argc, char* argv[]);
unsigned int Adler32(FILE *finn, unsigned int num);