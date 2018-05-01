#ifndef _STDIO_H
#define _STDIO_H
static const int EOF = -1;

#define FILE_COUNT 1000

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_DIRECTORY 3

typedef struct FILE
{
 int fd ;
 int flags;
 long offset;
} FILE ;

//initialise files here 
extern FILE all_files[FILE_COUNT];

#define stdin all_files[0]
#define stdout all_files[1]
#define stderr all_files[2]

//FILE lib
FILE * fopen(char *path, const char *permission);
int fputs(const char *buff, FILE* file_ptr);
int fgets(char *s, FILE *file_ptr, int size);
int fclose(FILE *file_ptr);

int putchar(int c);
int puts( char *s);
int puts1( const char *s);
char *gets(char *s);



#endif
