#ifndef _STRING_H
#define _STRING_H





int MyfindElement(char *s,char c);
int Mystrlen(const char *s);
int Mystrncmp(char *s1,char *s2,int n);

char * Mystrtok(char *str,char *delim);
char * Mystrcat(char *s1, char *s2);
int Mystrcmp(char *s1, char *s2);
#endif
