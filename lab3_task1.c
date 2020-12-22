#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

void openDir(const char *path);
void showFileData(const char* nameOfFile);
void getPermission(const int st_mode, char* permissions);

int main(char argc, char* argv[])
{
	const char *path;
	if (argc >= 2)
		path = argv[1];
	else
		path = ".";
	openDir(path);

	return 0;
}

void openDir(const char *path)
{
	struct dirent *entry;
	DIR *dir;
	char fullpath[PATH_MAX] = { 0 };
	if((dir = opendir(path)) == NULL)
	{
		fprintf(stderr, "Error in openDir: %s - %s\n", path, strerror(errno));
		return;
	}
	while((entry = readdir(dir)) != NULL)
	{
		sprintf(fullpath, "%s/%s", path, entry->d_name);
		showFileData(fullpath);
		fprintf(stdout, "\t\t%s\n", entry->d_name);
	}
	closedir(dir);
}

void showFileData(const char* nameOfFile)
{
  struct stat     statv;
  struct passwd  *pw_d;
  struct group   *gr;

  char username[256];
  char groupname[256];

  if (lstat(nameOfFile, &statv) == -1)
  {
	fprintf(stderr, "Can not open %s directory: %s\n", nameOfFile, strerror(errno));
	exit(EXIT_FAILURE);
    	return;
  }
  if((pw_d = getpwuid(statv.st_uid)) == NULL)
  {
	  sprintf(username, "%d", statv.st_uid);
  }
  else
  {
	  sprintf(username, "%s", pw_d->pw_name);
  }
  if((gr = getgrgid(statv.st_gid)) == NULL)
  {
	  sprintf(groupname, "%d", statv.st_gid);
  }
  else
  {
	  sprintf(groupname, "%s", gr->gr_name);
  }
  char permissions[10 + 1] = { 0 };
  getPermission(statv.st_mode, permissions);
  fprintf(stdout, "%11s%10ld%20s%25s\t\t%ld\t\t%.24s\t%ld", permissions, statv.st_nlink, username, groupname, statv.st_size,
		ctime(&statv.st_mtime), statv.st_ino);
}

void getPermission(const int st_mode, char* permissions)
{
	memset(permissions, '-', 10);

	switch (st_mode & S_IFMT)
	{
		case S_IFREG:  	permissions[0] = '-'; break;
        	case S_IFDIR:   permissions[0] = 'd'; break;
        	case S_IFCHR:   permissions[0] = 'c'; break;
        	case S_IFBLK:   permissions[0] = 'b'; break;
        	case S_IFLNK:   permissions[0] = 'l'; break;
        	case S_IFSOCK:  permissions[0] = 's'; break;
		case S_IFIFO:   permissions[0] = 'p'; break;
	}

	if(st_mode & S_IRUSR)
		permissions[1] = 'r';
        if(st_mode & S_IWUSR)
                permissions[2] = 'w';
        if(st_mode & S_IXUSR)
                permissions[3] = 'x';
        if(st_mode & S_IRGRP)
               	permissions[4] = 'r';
        if(st_mode & S_IWGRP)
                permissions[5] = 'w';
        if(st_mode & S_IXGRP)
                permissions[6] = 'x';
        if(st_mode & S_IROTH)
                permissions[7] = 'r';
        if(st_mode & S_IWOTH)
                permissions[8] = 'w';
        if(st_mode & S_IXOTH)
                permissions[9] = 'x';
}
