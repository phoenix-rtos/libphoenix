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
extern void *memchr(const void *str, int c, size_t n);


/* Compares the first n bytes of str1 and str2. */
extern int memcmp(const void *str1, const void *str2, size_t n);


/* Copies n characters from src to dest. */
extern void *memcpy(void *dest, const void *src, size_t n);


/* Another function to copy n characters from str2 to str1. */
extern void *memmove(void *dest, const void *src, size_t n);


/* Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str. */
extern void *memset(void *str, int c, size_t n);


/* Copies word v (an unsigned char) to the first n characters of the string pointed to, by the argument str. */
extern void *memsetw(void *str, int v, size_t n);


/* Appends the string pointed to, by src to the end of the string pointed to by dest. */
extern char *strcat(char *dest, const char *src);


/* Appends the string pointed to, by src to the end of the string pointed to, by dest up to n characters long. */
extern char *strncat(char *dest, const char *src, size_t n);


/* Searches for the first occurrence of the character c (an unsigned char) in the string pointed to, by the argument str. */
extern char *strchr(const char *str, int c);


/* Compares the string pointed to, by str1 to the string pointed to by str2. */ 
extern int strcmp(const char *str1, const char *str2);


/* Compares at most the first n bytes of str1 and str2. */
extern int strncmp(const char *str1, const char *str2, size_t n);


/* Compares string str1 to str2. The result is dependent on the LC_COLLATE setting of the location. */ 
extern int strcoll(const char *str1, const char *str2);


/* Copies the string pointed to, by src to dest. */
extern char *strcpy(char *dest, const char *src);


/* Copies up to n characters from the string pointed to, by src to dest. */
extern char *strncpy(char *dest, const char *src, size_t n);


/* Calculates the length of the initial segment of str1 which consists entirely of characters not in str2. */
extern size_t strcspn(const char *str1, const char *str2);


/* Searches an internal array for the error number errnum and returns a pointer to an error message string. */
extern char *strerror(int errnum);


/* Computes the length of the string str up to but not including the terminating null character. */
extern size_t strlen(const char *str);


/* Finds the first character in the string str1 that matches any character specified in str2. */
extern char *strpbrk(const char *str1, const char *str2);


/* Searches for the last occurrence of the character c (an unsigned char) in the string pointed to by the argument str. */
extern char *strrchr(const char *str, int c);


/* Calculates the length of the initial segment of str1 which consists entirely of characters in str2. */
extern size_t strspn(const char *str1, const char *str2);


/*
 * Finds the first occurrence of the entire string needle (not including the terminating null character)
 * which appears in the string haystack.
 */
extern char *strstr(const char *haystack, const char *needle);


/* Breaks string str into a series of tokens separated by delim. */
extern char *strtok(char *str, const char *delim);


/* Transforms the first n characters of the string src into corrent locale and places them in the string dest. */
extern size_t strxfrm(char *dest, const char *src, size_t n);


#endif
