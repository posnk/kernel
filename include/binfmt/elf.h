/******************************************************************************\
Copyright (C) 2015,2016 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
* @file include/binfmt/elf.h
*
* Part of posnk kernel
*
* Written by Peter Bosch <peterbosc@gmail.com>
*
*/

#ifndef __binfmt_elf_h__
#define __binfmt_elf_h__

#include <string.h>

#define EI_NIDENT 16

#define ET_NONE		0	/** No file type */
#define ET_REL		1	/** Relocatable file */
#define ET_EXEC		2	/** Executable file */
#define ET_DYN		3	/** Shared object file */
#define ET_CORE		4	/** Core file */
#define ET_LOOS		0xff00 	/** Start OS specific */
#define ET_HIOS		0xffff 	/** End OS specific */
#define ET_LOPROC	0xff00 	/** Start ARCH specific */
#define ET_HIPROC	0xffff 	/** End ARCH specific */

#define EM_NONE		0	/** No machine */
#define EM_M32		1	/** AT&T WE 32100 */
#define EM_SPARC	2	/** Sun SPARC */
#define EM_386		3	/** Intel 80386 (i386, x86) */
#define	EM_68K		4	/** Motorola 68000 */
#define EM_88K		5	/** Motorola 88000 */
#define EM_860		7	/** Intel 80860 */
#define EM_MIPS		8	/** MIPS RS3000 */
#define EM_ARM		0x28	/** ARM */

#define	EV_NONE		0	/* Invalid version */
#define EV_CURRENT	1	/* Current version */

#define EI_MAG0         0	/* e_ident[] indexes */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4	/** The class of the ELF object */
#define EI_DATA         5	/** Encoding of processor specific data */
#define EI_VERSION      6	/** The ELF header version */
#define EI_OSABI        7	/** The Operating System ABI the obj targets*/
#define EI_PAD          8	

#define ELFMAG          "\177ELF"
#define SELFMAG         4

#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2

#define ELFDATANONE	0
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

#define ELFOSABI_NONE   0
#define ELFOSABI_POS    17

/* special section indexes */
#define SHN_UNDEF       0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC      0xff00
#define SHN_HIPROC      0xff1f
#define SHN_ABS         0xfff1		/* Symbol value unaffected by reloc */
#define SHN_COMMON      0xfff2	
#define SHN_HIRESERVE   0xffff

/* sh_type */
#define SHT_NULL        0		/* inactive section */
#define SHT_PROGBITS    1		/* program defined data */
#define SHT_SYMTAB      2		/* symbol table, full */
#define SHT_STRTAB      3		/* string table */
#define SHT_RELA        4		/* relocation table, explicit */
#define SHT_HASH        5		/* symbol hash table */
#define SHT_DYNAMIC     6		/* dynamic linking info */
#define SHT_NOTE        7		/* misc... */
#define SHT_NOBITS      8		/* program defined data, 
					   not in file, zerofill */
#define SHT_REL         9		/* relocation table */
#define SHT_SHLIB       10		/* unspecified */
#define SHT_DYNSYM      11		/* symbol table, compact for dynlink */
#define SHT_NUM         12		
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

/* sh_flags */
#define SHF_WRITE       0x1		/* Writable section */
#define SHF_ALLOC       0x2		/* Section should be in RAM */
#define SHF_EXECINSTR   0x4		/* Executable instructions in section */
#define SHF_MASKPROC    0xf0000000	/* Processor specific */

/* st_info */
#define ELF32_ST_BIND(i)	((i)>>4)
#define ELF32_ST_TYPE(i)	((i)&0xf)
#define ELF32_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

/* st_info->bind */
#define STB_LOCAL	0		/* Local symbol */
#define STB_GLOBAL	1		/* Global symbol */
#define STB_WEAK	2		/* Weak, global symbol */
#define STB_LOPROC	13		/* Start of arch specific */
#define STB_HIPROC	15		/* End of arch specific */

/* st_info->type */
#define STT_NOTYPE	0		/* Undefined type */
#define STT_OBJECT	1		/* Data object */
#define STT_FUNC	2		/* Function/code */
#define STT_SECTION	3		/* Symbol assoc to section */
#define STT_FILE	4		/* Source unit name */
#define STT_LOPROC	13		/* Start of arch specific */
#define STT_HIPROC	15		/* End of arch specific */

/* r_info */
#define ELF32_R_SYM(i)		((i)>>8)
#define ELF32_R_TYPE(i)		((unsigned char)(i))
#define ELF32_R_INFO(s,t)	(((s)<<8)+(unsigned char)(t))

/* r_type */
#define R_386_NONE	0		/* No relocation */
#define R_386_32	1		/* symbol + addend */
#define R_386_PC32	2		/* symbol + addend - ptr */
#define R_386_GOT32	3		/* gotaddr + addend - ptr */
#define R_386_PLT32	4		/* linktbl + addend - ptr */
#define R_386_COPY	5		/* copy data from so */
#define R_386_GLOB_DAT	6		
#define R_386_JMP_SLOT	7
#define R_386_RELATIVE	8
#define R_386_GOTOFF	9
#define R_386_GOTPC	10

/* These constants are for the segment types stored in the image headers */
#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_TLS		7               /* Thread local storage segment */
#define PT_LOOS		0x60000000      /* OS-specific */
#define PT_HIOS		0x6fffffff      /* OS-specific */
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff

#define PF_X		0x1
#define PF_W		0x2
#define PF_R		0x4

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef  int32_t	Elf32_Sword;
typedef uint32_t	Elf32_Word;

typedef struct {
        unsigned char e_ident[EI_NIDENT]; 
        Elf32_Half    e_type;		/** The object file type */
        Elf32_Half    e_machine;	/** The machine targeted */
        Elf32_Word    e_version;	/** The version */ 
        Elf32_Addr    e_entry;		/** Entry point virt_addr */
        Elf32_Off     e_phoff;		/** Program header offset */
        Elf32_Off     e_shoff;		/** Section header offset */
        Elf32_Word    e_flags;		/** Processor specific flags */
        Elf32_Half    e_ehsize;		/** ELF header size */
        Elf32_Half    e_phentsize;	/** Program header entry size */
        Elf32_Half    e_phnum;		/** Program header entry count */
        Elf32_Half    e_shentsize;	/** Section header entry size */
        Elf32_Half    e_shnum;		/** Section header entry count */
        Elf32_Half    e_shstrndx;	/** Section header string table idx */
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	sh_name;	/* Name string table index */
	Elf32_Word	sh_type;	/* Section type */
	Elf32_Word	sh_flags;	/* Section flags */
	Elf32_Addr	sh_addr;	/* Section virt_addr */
	Elf32_Off	sh_offset;	/* Section data file offset */
	Elf32_Word	sh_size;	/* Section size */
	Elf32_Word	sh_link;	/* Section header table link */
	Elf32_Word	sh_info;	/* Extra information */	
	Elf32_Word	sh_addralign;	/* Address alignment constrt */
	Elf32_Word	sh_entsize;	/* Entry size */
} Elf32_Shdr;

typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct {
	Elf32_Word	st_name;	/* String table index of name */
	Elf32_Addr	st_value;	/* The value of the symbol */
	Elf32_Word	st_size;	/* The size of the symbol */
	unsigned char	st_info;	/* The type and binding info */
	unsigned char	st_other;	/* No meaning yet */
	Elf32_Half	st_shndx;	/* Section index */
} Elf32_Sym;

typedef struct {
	Elf32_Addr	r_offset;	/* The offset to relocate */
	Elf32_Word	r_info;		/* Extra information */
} Elf32_Rel;

typedef struct {
	Elf32_Addr	r_offset;	/* The offset to relocate */
	Elf32_Word	r_info;		/* Extra information */
	Elf32_Sword	r_addend;	/* Constant addend to use */
} Elf32_Rela;

int	elf_verify_ident( Elf32_Ehdr * hdr );
int	elfmem_load_segments(void *start, size_t size);

#endif

