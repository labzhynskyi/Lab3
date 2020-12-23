#include <unistd.h> // for POSIX constants
#include <stdlib.h> // for working with memory
#include <string.h> // for string type 
#include <stdio.h> // for sprintf
#include <sys/types.h> // for stat struct
#include <sys/stat.h> // stat struct
#include <dirent.h> // for dirent*, DIR*
#include <errno.h> // for errors
#include <limits.h>

#define MAX_PATH_LENGTH 1024 // максимальная длина пути к файлу

void openDirectory(const char* path, unsigned long int* size); // эта функция открывает каталог и вызывает fsize - функцию размера одного файла
void fsize(const char* name, unsigned long int* size, const char*  nameWithoutPath); // эта функция считает размер файла, переданного в аргумент

int main(int argc, char* argv[])
{
	unsigned long int size = 0; // эта переменная для размера всех файлов в каталоге
	if(argc > 1)
	{
		for(int i = argc - 1;i > 0; i--) // Обход всех параметров, переданных в функцию
		{
			openDirectory(argv[i], &size);// вызываем функцию openDirectory и передаем в нее путь к заданой директории  и адрес переменной размера
		}
	}
	else
	{
		openDirectory(".", &size); // вызываем функцию openDirectory и передаем в нее путь к текущей директории (".") и адрес переменной размера
	}
	printf("\nSize of files = %ld\n", size); // выводим размер директории
	return 0;
}

void openDirectory(const char* path, unsigned long int* size)
{
	struct dirent* entry; // указатель на специальную структуру (тип struct dirent) для каждого элемента каталога
	DIR* dir; //для работы с каталогами нам нужно определить переменную типа DIR
	char fullPath[MAX_PATH_LENGTH] = {0}; // full path to file

	if((dir = opendir(path)) == NULL) //если эта функция возвращает ошибку
	{
		fprintf(stderr, "Error: %s\n", strerror(errno)); // выводим сообщение об ошибке
		return;
	}
	while((entry = readdir(dir)) != NULL)//readdir() возвращает следующую структуру dirent, считанную из файла-директории. При достижении конца списка файлов в директории или возникновении ошибки возвращает NULL
	{
		sprintf(fullPath, "%s/%s", path, entry->d_name); //в переменную fullPath записываем путь, переданный в эту функцию и имя файла
		fsize(fullPath, size, entry->d_name); // передаем полный путь к файлу и переменную, в которой считается размер и имя файла
		//адрес не берем, так как size - уже указатель
	}
	closedir(dir);
}

void fsize(const char* name, unsigned long int* size, const char*  nameWithoutPath)
{
	//если размер полного пути к файлу больше заданного по условию, то функция fsize не считает размер этого файла
	if(strlen(name) > MAX_PATH_LENGTH)
	{
		printf("The file name cann`t be greater than 1024!\n");
		return;
	}
	struct stat statv;
	if(lstat(name, &statv) == -1) // lstat идентичен stat, но в случае символических ссылок он возвращает информацию о самой ссылке, а не о файле, на который она указывает
	{
		fprintf(stderr, "Error - %s\n", strerror(errno)); // отобразить сообщение об ошибке
		return;//выполнение в этой функции заканчивается, но ошибка не критическая, поэтому работа программы продолжается
	}
	switch(statv.st_mode & S_IFMT)
	{
		case S_IFDIR: // directory
			if((strcmp(nameWithoutPath, ".") == 0) || (strcmp(nameWithoutPath, "..") == 0))
			{
				break; // пропускаем
			}
			*size += statv.st_size;
			openDirectory(name, size); // открыть каталог
			break;
		case S_IFREG: //если обычный файл - то просто считываем размер файла
		case S_IFLNK:
			*size += statv.st_size; // записываем размер файла во временную переменную
			break;
	}
}
