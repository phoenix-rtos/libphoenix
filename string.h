/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STRING_H_
#define _LIBPHOENIX_STRING_H_

#include ARCH


/*
 * Searches for the first occurrence of the character c (an unsigned char) in the first n bytes of the string
 * pointed to, by the argument str.
 */
#ifndef MEMCHR
extern void *memchr(const void *str, int c, size_t n);
#endif


/* Compares the first n bytes of str1 and str2. */
#ifndef MEMCMP
int memcmp(const void *str1, const void *str2, size_t n);
#endif

/* Copies n characters from src to dest. */
#ifndef MEMCPY
void *memcpy(void *dest, const void *src, size_t n);
#endif


/* Another function to copy n characters from str2 to str1. */
#ifndef MEMMOVE
void *memmove(void *dest, const void *src, size_t n);
#endif


/* Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str. */
#ifndef MEMSET
extern void *memset(void *str, int c, size_t n);
#endif


/* Appends the string pointed to, by src to the end of the string pointed to by dest. */
#ifndef STRCAT
extern char *strcat(char *dest, const char *src);
#endif


/* Appends the string pointed to, by src to the end of the string pointed to, by dest up to n characters long. */
#ifndef STRNCAT
extern char *strncat(char *dest, const char *src, size_t n);
#endif


/* Searches for the first occurrence of the character c (an unsigned char) in the string pointed to, by the argument str. */
#ifndef STRCHR
extern char *strchr(const char *str, int c);
#endif


/* Compares the string pointed to, by str1 to the string pointed to by str2. */ 
#ifndef STRCMP
extern int strcmp(const char *str1, const char *str2);
#endif


/* Compares at most the first n bytes of str1 and str2. */
#ifndef STRNCMP
extern int strncmp(const char *str1, const char *str2, size_t n);
#endif


/* Compares string str1 to str2. The result is dependent on the LC_COLLATE setting of the location. */ 
#ifndef STRCOLL
extern int strcoll(const char *str1, const char *str2);
#endif


/* Copies the string pointed to, by src to dest. */
#ifndef STRCPY	
extern char *strcpy(char *dest, const char *src);
#endif


/* Copies up to n characters from the string pointed to, by src to dest. */
#ifndef STRNCPY
extern char *strncpy(char *dest, const char *src, size_t n);
#endif


/* Calculates the length of the initial segment of str1 which consists entirely of characters not in str2. */
#ifndef STRCSPN
extern size_t strcspn(const char *str1, const char *str2);
#endif


/* Searches an internal array for the error number errnum and returns a pointer to an error message string. */
#ifndef STRERROR
extern char *strerror(int errnum);
#endif


/* Computes the length of the string str up to but not including the terminating null character. */
#ifndef STRLEN
extern size_t strlen(const char *str);
#endif


/* Finds the first character in the string str1 that matches any character specified in str2. */
#ifndef STRPBRK
extern char *strpbrk(const char *str1, const char *str2);
#endif


/* Searches for the last occurrence of the character c (an unsigned char) in the string pointed to by the argument str. */
#ifndef STRRCHR
extern char *strrchr(const char *str, int c);
#endif


/* Calculates the length of the initial segment of str1 which consists entirely of characters in str2. */
#ifndef STRSPN
extern size_t strspn(const char *str1, const char *str2);
#endif


/*
 * Finds the first occurrence of the entire string needle (not including the terminating null character)
 * which appears in the string haystack.
 */
#ifndef STRSTR	
extern char *strstr(const char *haystack, const char *needle);
#endif


/* Breaks string str into a series of tokens separated by delim. */
#ifndef STRTOK
extern char *strtok(char *str, const char *delim);
#endif


/* Transforms the first n characters of the string src into corrent locale and places them in the string dest. */
#ifndef STRXFRM
extern size_t strxfrm(char *dest, const char *src, size_t n);
#endif


#endif
