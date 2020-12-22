#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>

#define MAX_PATH_LENGTH 1024

void openDir(const char* path, uintmax_t *size);
void fsize(const char* name, uintmax_t *size,const char* fileName);

int main(int argc, char* argv[])
{
	uintmax_t size = 0; // uintmax_t is at least 64 bits
	if(argc > 1)
	{
		for(int i = argc - 1;i > 0; i--)
		{
			fsize(argv[i], &size, argv[i]);
		}
	}
	else
	{
		openDir(".", &size);
	}
	printf("Size of files = %ld\n", size);
	return 0;
}

void openDir(const char* path, uintmax_t *size)
{
	struct dirent* entry;
	DIR* dir;
	char fullPath[PATH_MAX] = {0};

	if((dir = opendir(path)) == NULL)
	{
		fprintf(stderr, "%s - %s\n", path, strerror(errno));
		return;
	}
	while((entry = readdir(dir)) != NULL)
	{
		sprintf(fullPath, "%s/%s", path, entry->d_name);
		fsize(fullPath, size, entry->d_name);
	}
	closedir(dir);
}

void fsize(const char* name, uintmax_t *size, const  char* fileName)
{
	if(strlen(name) > MAX_PATH_LENGTH)
	{
		printf("The file name can not be greater than 1024\n");
		return;
	}
	unsigned long int tempSize = 0;
	struct stat statv;
	if(lstat(name, &statv) == -1)
	{
		fprintf(stderr, "Can not open %s file: %s\n", name, strerror(errno));
		return;
	}
	char fileType; // find file type
	switch (statv.st_mode & S_IFMT)
	{
		case S_IFREG:  	fileType = '-'; break;
        	case S_IFDIR:   fileType = 'd'; break;
        	case S_IFCHR:   fileType = 'c'; break;
        	case S_IFBLK:   fileType = 'b'; break;
        	case S_IFLNK:   fileType = 'l'; break;
        	case S_IFSOCK:  fileType = 's'; break;
		case S_IFIFO:   fileType = 'p'; break;
	}
	switch(fileType)
	{
		case 'd':
			if((strcmp(fileName, ".") == 0) || (strcmp(fileName, "..") == 0))
			{
				break; // skip these folders
			}
			tempSize = statv.st_size;
			openDir(name, size);
			break;
		case '-':
			tempSize = statv.st_size;
			break;
		case 'l':
			tempSize = statv.st_size;
			break;
		default :
			tempSize = 0;
	}
	*size += tempSize;
}
