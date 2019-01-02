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
	display_print_to_screen("Hello, %s!\r\n", "firmware");
	
	return 0;
}

static void custom_print(char c)
{
	// Replace by your custom putchar (uart_writechar(), lcd_write_char() ...)
	putchar(c);   
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



