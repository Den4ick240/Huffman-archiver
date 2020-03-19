#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "coder.h"
#include "decoder.h"
#include "FileLIst.h"
#include "Checks.h"

#define MAX_FILE_NAME_SIZE 256

#define ERROR_BAD_ARG {printf("Error: Bad arguments\n");return 1;}
#define ERROR_BAD_EXT {printf("Error: Bad archive extension.\nThe extension have to be .arch\n");return 1;}
#define ERROR_FILE_NOT_FOUND(n) {printf("Error: file number %d not found.\n", n);return 1;}
#define ERROR_ARCHIVE_NOT_FOUND {printf("Error: archive not found. Check the archive name.\n");return 1;}

void Print_help()
{
	printf("Tihs is a simple archiver made as an educational\n");
	printf("project on programming by a first-year student of\n");
	printf("the faculty of information technologies of Novosibirsk\n");
	printf("state University.\n\n");

	printf("Guide to use:\n");

	printf("To add file to an archive, pass the following arguments:\n");
	printf("\t-a <archive_name> <first_file_name> ... <last_file_name>\n\n");

	printf("To extract file from an archive to the same dir as the ");
	printf("archive, pass the following arguments:\n");
	printf("\t-x <archive_name> <first_file_name> ... <last_file_name>\n");
	printf("If there is no following file names, all files will be extracted\n\n");

	printf("To delete file from an archive, pass the following arguments:\n");
	printf("\t-d <archive_name> <first_file_name> ... <last_file_name>\n");
	printf("If there is no following file names or file to be deleted is the last one, archive will be deleted\n\n");

	printf("To see the inormation about files in the archive, pass the following arguments:\n");
	printf("\t-l <archive_name>\n\n");

	printf("To check the integrity of archive, pass the following arguments:\n");
	printf("\t-t <archive_name>\n\n");

	printf("Archive extension have to be .arch\n");
	printf("You can't add any folder to an archive :(\n");
	printf("Enter exit if you want to end the programm.\n");
}

int Add(int argc, char* argv[])
{
	File_in_arch	*file_list = NULL;
	FILE			*arch = NULL;
	FILE			*finn = NULL;
	unsigned int	num_files;
	unsigned int	repeats = 0;
	int				i;

	i = CheckFilesExistence(argc, argv);
	if (i) ERROR_FILE_NOT_FOUND(i - 2);

	arch = fopen(argv[2], "r");
	if (arch)
	{
		int flag = 0;
		fclose(arch);
		arch = fopen(argv[2], "r+b");
		file_list = ScanFileList(arch, &num_files, argc - 3);
		fseek(arch, file_list[num_files - 1].end, SEEK_SET);
		for (i = 3; i < argc; i++)
		{
			unsigned int idx = 0;
			if (FindFileInList(file_list, num_files, argv[i], &idx))
			{
				flag++;
				printf("Error: File named %s is already in archive.\n", file_list[idx].name);
			}
		}
	}
	else
	{
		arch = fopen(argv[2], "w+b");
		file_list = (File_in_arch*)malloc(sizeof(File_in_arch) * (argc - 3));
		num_files = 0;
	}

	for (i = 3; i < argc; i++)
	{
		if (FindFileInList(file_list, num_files, argv[i], NULL) || !strcmp(argv[i], argv[2]))
		{
			repeats++;
			continue;
		}
		finn = fopen(argv[i], "rb");
		fseek(finn, 0, SEEK_END);
		file_list[num_files].file_size = ftell(finn);
		fseek(finn, 0, SEEK_SET);
		file_list[num_files].name = argv[i];
		file_list[num_files].size = strlen(argv[i]);
		file_list[num_files].start = ftell(arch);
		Encode(finn, arch);
		file_list[num_files++].end = ftell(arch);
		fclose(finn);
	}
	PrintFileList(file_list, num_files, arch);

	FreeFileList(file_list, num_files - argc + repeats + 3);
	fclose(arch);

	return 0;
}

int Extract(int argc, char* argv[])
{
	unsigned int	num_files = 0;
	unsigned int	i;
	File_in_arch	*file_list;
	FILE			*arch = NULL;
	FILE			*finn = NULL;

	arch = fopen(argv[2], "r+b");
	if (arch == NULL)
		ERROR_ARCHIVE_NOT_FOUND;

	file_list = ScanFileList(arch, &num_files, 0);

	for (i = 3; i < argc; i++)
		if (!FindFileInList(file_list, num_files, argv[i], NULL))
			printf("Error: File %s havent been found.\n", argv[i]);

	for (i = 0; i < num_files; i++)
	{
		if (argc > 3)
		{
			int j;
			for (j = 3; j < argc; j++)
				if (!strcmp(file_list[i].name, argv[j]))
					break;
			if (j == argc)
				continue;
		}
		finn = fopen(file_list[i].name, "rb");
		if (finn)
		{
			char ans[16];
		POINTER:
			printf("File %s already exists. Do you want to replase it?(enter yes or no)\n", file_list[i].name);
			fgets(ans, 16, stdin);
			printf("\n");
			if (!strcmp(ans, "no\n"))
			{
				fclose(finn);
				continue;
			}
			else if (strcmp(ans, "yes\n"))
				goto POINTER;
			fclose(finn);
		}
		finn = fopen(file_list[i].name, "wb");
		fseek(arch, file_list[i].start, SEEK_SET);
		Decode(arch, finn);
		fclose(finn);
		printf("File %s has been extracted.\n", file_list[i].name);
	}

	FreeFileList(file_list, num_files);
	fclose(arch);
	return 0;
}

int Delete(int argc, char* argv[])
{
	File_in_arch	*file_list;
	FILE			*arch = NULL;
	FILE			*tmp_file;
	unsigned int	j;
	unsigned int	i;
	unsigned int	num_files;
	char			tmp_name[L_tmpnam];

	arch = fopen(argv[2], "r+b");
	if (arch == NULL)
		ERROR_ARCHIVE_NOT_FOUND;

	if (argc == 3)
	{
	DELETE_ARCHIVE:
		while (1)
		{
			char ans[5];
			printf("Do you want to delete the entire archive? Enter yes or no.\n");
			fgets(ans, 5, stdin);
			printf("\n");
			if (!strcmp("yes\n", ans))
			{
				remove(argv[2]);
				printf("%s has been removed.\n", argv[2]);
				fclose(arch);
				return 0;
			}
			else if (!strcmp("no\n", ans))
			{
				fclose(arch);
				return 1;
			}
		}
	}

	file_list = ScanFileList(arch, &num_files, 0);
	if (num_files <= (argc - 3))
	{
		printf("The number of specified files is greater than or equal to the number of files in the archive.\n");
		FreeFileList(file_list, num_files);
		fclose(arch);
		goto DELETE_ARCHIVE;
		/*remove(argv[2]);
		printf("%s has been removed.\n", argv[2]);
		return 0;*/
	}

	for (i = 3; i < argc; i++)
	{
		for (j = 0; j < num_files; j++)
			if (!strcmp(argv[i], file_list[j].name))
			{
				file_list[j].size = 0;
				break;
			}
		if (j == num_files)
			printf("%s hasn't been found in the archive.\n", argv[i]);
	}

	tmpnam(tmp_name);
	tmp_file = fopen(tmp_name, "w+b");
	for (i = 0; i < num_files; i++)
	{
		char *buff;
		if (file_list[i].size == 0)
			continue;

		j = file_list[i].end - file_list[i].start;
		buff = (char*)malloc(j);

		fseek(arch, file_list[i].start, SEEK_SET);
		fread(buff, 1, j, arch);

		file_list[i].start = ftell(tmp_file);
		fwrite(buff, 1, j, tmp_file);
		file_list[i].end = ftell(tmp_file);

		free(buff);
	}

	PrintFileList(file_list, num_files, tmp_file);

	fclose(arch);
	fclose(tmp_file);
	remove(argv[2]);
	rename(tmp_name, argv[2]);

	printf("Removing finished.\n");

	FreeFileList(file_list, num_files);
	return 0;
}

int Info(int argc, char* argv[])
{
	FILE			*arch = NULL;
	File_in_arch	*file_list;
	unsigned int	num_files;
	unsigned int	i;
	unsigned int	max_name_len;
	unsigned int	table_width;
	unsigned int	sum_arch = 0;
	unsigned int	sum_act = 0;
	unsigned int	size_arch;
	unsigned int	size_act;
	char			line[1024] = { 0 };
	char			name_line[512] = { 0 };
	char			titles[5][32] = { "Name", "Size in archive", "Actual size", "In Total", "File List" };
	char			name_line_width;

	arch = fopen(argv[2], "rb");
	if (arch == NULL)
		ERROR_ARCHIVE_NOT_FOUND;

	file_list = ScanFileList(arch, &num_files, 0);

	max_name_len = strlen(titles[4]);
	for (i = 0; i < num_files; i++)
		if (file_list[i].size > max_name_len)
			max_name_len = file_list[i].size;

	table_width = 1 + max_name_len + 1 + strlen(titles[1]) + 1 + strlen(titles[2]) + 1;
	for (i = 0; i < table_width; i++)
		line[i] = '=';
	line[table_width] = 0;

	printf("\n\n%s\n", line);
	name_line_width = max_name_len - strlen(titles[0]);
	for (i = 0; i < name_line_width; i++)
		name_line[i] = ' ';
	name_line[name_line_width] = 0;
	printf("|%s%s|%s|%s|\n", titles[0], name_line, titles[1], titles[2]);
	printf("%s\n", line);

	for (i = 0; i < num_files; i++)
	{
		name_line_width = max_name_len - file_list[i].size;
		name_line[name_line_width] = 0;

		while (name_line_width-- > 0)
			name_line[name_line_width] = ' ';

		sum_act += size_act = file_list[i].file_size;
		sum_arch += size_arch = file_list[i].end - file_list[i].start;

		printf("|%s%s|%*ub|%*ub|\n", file_list[i].name, name_line, strlen(titles[1]) - 1, size_arch, strlen(titles[2]) - 1, size_act);
		printf("%s\n", line);
	}

	sum_act += size_act = 0;
	sum_arch += size_arch = ftell(arch) - file_list[i - 1].end + 4/*hash code*/;

	name_line_width = max_name_len - strlen(titles[4]);
	name_line[name_line_width] = 0;
	while (name_line_width-- > 0)
		name_line[name_line_width] = ' ';

	printf("|%s%s|%*ub|%*ub|\n", titles[4], name_line, strlen(titles[1]) - 1, size_arch, strlen(titles[2]) - 1, size_act);
	printf("%s\n", line);

	name_line_width = max_name_len - strlen(titles[3]);
	name_line[name_line_width] = 0;
	while (name_line_width-- > 0)
		name_line[name_line_width] = ' ';

	printf("|%s%s|%*ub|%*ub|\n", titles[3], name_line, strlen(titles[1]) - 1, sum_arch, strlen(titles[2]) - 1, sum_act);
	printf("%s\n", line);

	printf("\n");
	fclose(arch);
	FreeFileList(file_list, num_files);

	return 0;
}

int Command(int argc, char* argv[])
{
	FILE *test_arch = NULL;
	if (argv[1][0] != '-' || argc == 2)
		ERROR_BAD_ARG;
	if (CheckExt(argv[2]))
		ERROR_BAD_EXT;
	test_arch = fopen(argv[2], "rb");
	if (test_arch != NULL)
	{
		fclose(test_arch);
		if (CheckIntegrity(argv[2]))
			return 1;
	}
	switch (argv[1][1])
	{
	case 'a':
		if (argc == 3)
			ERROR_BAD_ARG;
		return Add(argc, argv);
		break;
	case 'x':
		return Extract(argc, argv);
		break;
	case 'd':
		return Delete(argc, argv);
		break;
	case 'l':
		return Info(argc, argv);
		break;
	case 't':
		return 0;
		break;
	default:
		ERROR_BAD_ARG;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc == 1 || !strcmp(argv[1], "help"))
	{
		Print_help();

		while (1)
		{
			int		argc;
			int		i = -1;
			int		j = 0;
			char	**argv = NULL;
			char	in[256];

			fgets(in, 256, stdin);
			if (in[0] == '\n')
			{
				printf("Enter 'exit' if you want to close the program.\n");
				continue;
			}
			if (!strcmp(in, "exit\n"))
				break;

			argc = 1;
			argv = (char**)malloc(sizeof(char*));

			do {
				if (in[i] == ' ' || in[i] == '\n')
				{
					argv = (char**)realloc(argv, ++argc * sizeof(char*));
					argv[argc - 1] = (char*)malloc(sizeof(char) * (i - j + 2));
					memcpy(argv[argc - 1], in + j, i - j);
					argv[argc - 1][i - j] = 0;
					j = i + 1;
				}
			} while (in[i++] != '\n');

			printf("Execute...\n\n");
			if (Command(argc, argv))
				printf("Operation canseled.\n");
			else
				printf("Operarion finished.\n");

			for (i = 1; i < argc; i++)
				free(argv[i]);
			free(argv);
		}
	}
	else
	{
		printf("Execute...\n");
		if (Command(argc, argv))
			printf("Operation canseled.\n");
		else
			printf("Operarion finished.\n");
	}

	printf("Press any key to close the window.\n");
	_getch();
	return 0;
}