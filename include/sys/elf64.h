

#ifndef _ELF64_H
#define _ELF64_H

#define EI_NIDENT 16
/*
typedef Elf64_Addr   uint64_t;
typedef Elf64_Half   uint16_t;
typedef Elf64_Lword  uint64_t;
typedef Elf64_Off    uint64_t;
typedef Elf64_Sword  uint32_t;
typedef Elf64_Sxword uint64_t;
typedef Elf64_Word   uint32_t;
typedef Elf64_Xword  uint64_t;
*/
typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t  e_type;
  uint16_t  e_machine;
  uint32_t  e_version;
  uint64_t  e_entry;
  uint64_t  e_phoff;
  uint64_t  e_shoff;
  uint32_t  e_flags;
  uint16_t  e_ehsize;
  uint16_t  e_phentsize;
  uint16_t  e_phnum;
  uint16_t  e_shentsize;
  uint16_t  e_shnum;
  uint16_t  e_shstrndx;
} Elf64_Ehdr;


typedef struct elf64_shdr {
  uint32_t sh_name;		/* Section name, index in string tbl */
  uint32_t sh_type;		/* Type of section */
  uint64_t sh_flags;		/* Miscellaneous section attributes */
  uint64_t sh_addr;		/* Section virtual addr at execution */
  uint64_t sh_offset;		/* Section file offset */
  uint64_t sh_size;		/* Size of section in bytes */
  uint32_t sh_link;		/* Index of another section */
  uint32_t sh_info;		/* Additional section information */
  uint64_t sh_addralign;	/* Section alignment */
  uint64_t sh_entsize;	/* Entry size if section holds table */
} Elf64_Shdr;


typedef struct {
  uint32_t  p_type;
  uint32_t  p_flags;
  uint64_t  p_offset;
  uint64_t  p_vaddr;
  uint64_t  p_paddr;
  uint64_t  p_filesz;
  uint64_t  p_memsz;
  uint64_t  p_align;
} Elf64_Phdr;

#endif
