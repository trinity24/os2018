#ifndef _STRING_H
#define _STRING_H

int strlen(const char *s);
int strcmp(char *s1, char *s2,uint64_t n);
int strcmp1(char *s1, char *s2);
int first_occurence(char *s,char c);
char *strtok(char *str,char *delim, char* token);
void strcat(char *s,char *s1, char *s2);
char *strcpy(char *dest, char *src);

#endif
