#include "arch/i386/x86.h"
#include <stddef.h>

uint8_t i386_inb(uint16_t port)
{
	unsigned char ret;
	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
}

void i386_outb(uint16_t port,uint8_t value)
{
	asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

uint16_t i386_inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %%dx,%%ax":"=a" (ret):"d" (port));
	return ret;
}

void i386_outw(uint16_t port,uint16_t value)
{
	asm volatile ("outw %%ax,%%dx": :"d" (port), "a" (value));
}

uint32_t i386_inl(uint16_t port)
{
	uint32_t ret;
	asm volatile ("inl %%dx,%%eax":"=a" (ret):"d" (port));
	return ret;
}

void i386_outl(uint16_t port,uint32_t value)
{
	asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}


void halt() 
{
	__asm__("cli;hlt");
}

