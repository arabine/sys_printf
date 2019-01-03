# A context-based embedded printf() library without buffer

## Introduction

Embedded systems usually need some debug interface to print internal firmware information. These interfaces can be quite exotic (UART, LCD, RF ...). Here is a proposal of an implementation of an optimized printf() function that allows to output data to various physical interfaces with a unique code base.

To achieve that, the library uses a specific context per physical interface. One more feature of this library is that it does NOT have any working big buffer to store characters. All characters are pushed immediately. 

Usage example:

```c
/**
BSD 3-Clause License

Copyright (c) 2018, Anthony Rabine
All rights reserved.
*/

#include <stdio.h>

#include "sys_printf.h"

static void display_print_to_screen(const char *format, ...);

int main(void)
{
	display_print_to_screen("Hello, %s!\n", "firmware");
	
	// Also snprintf equivalent
	char buf[255] = {0};
	
	sys_snprintf(&buf[0], sizeof(buf), "Tesla model 3 price is: $%d", 50000);	
	
	puts(buf);
	
	return 0;
}

static void custom_print(char c)
{
	// Replace by your custom putchar (uart_writechar(), lcd_write_char() ...)
	
	if (c != 0) // beware, zero is generated for string terminaison
	{
		putc(c, stdout); 
	}
}

static void display_print_to_screen(const char *format, ...)
{
	sys_print_ctx_t printCtx; // our printf context
	
	printCtx.pPutc = custom_print; // specific putchar for that interface
	printCtx.len = 0; // init to zero, character counter
	printCtx.maxLen = 255; // max length to push
	
	va_list arg;
    va_start(arg, format);

    (void)sys_printf(&printCtx, 0, format, arg);
    va_end(arg);
}
```
