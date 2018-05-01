#ifndef _STRING_H
#define _STRING_H

int strlen(const char *s);
int strcmp(char *s1, char *s2,uint64_t n);
int strcmp1(char *s1, char *s2);
int first_occurence(char *s,char c);
char *strtok(char *str,char *delim, char* token);
char * strcat(char *s1, char *s2);
#endif
