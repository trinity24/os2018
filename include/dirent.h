#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255
typedef struct dirent dirent;

struct dirent {
 char d_name[NAME_MAX+1];
};

struct linux_dirent {
   long d_ino;
   off_t d_off;
   unsigned short d_reclen;
   char d_name[];
};

typedef struct DIR DIR;

struct DIR
{
     int fd;                     /* File descriptor.  */
     char dir_name[NAME_MAX+1];
     dirent direntry;

};

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count);

#endif
