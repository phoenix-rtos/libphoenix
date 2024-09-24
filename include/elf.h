/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * elf.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ELF_H
#define _LIBPHOENIX_ELF_H

#include <phoenix/elf.h>

/* NOTE: many structs have only definitions in phoenix for one of the sizes, thus common interface is missing */
#if __SIZEOF_POINTER__ == 4

typedef Elf32_Half Elf_Half;
typedef Elf32_Word Elf_Word;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Off Elf_Off;
typedef Elf32_Sword Elf_Sword;

typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Phdr Elf_Phdr;

#else

typedef Elf64_Half Elf_Half;
typedef Elf64_Word Elf_Word;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Off Elf_Off;
typedef Elf64_Sword Elf_Sword;

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Phdr Elf_Phdr;

#endif


#endif /* _LIBPHOENIX_ELF_H */
