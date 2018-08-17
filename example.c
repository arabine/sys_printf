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
	putchar(c); 
}

static void display_print_to_screen(const char *format, ...)
{
	sys_print_ctx_t printCtx;
	
	printCtx.pPutc = custom_print;
	printCtx.len = 0;
	printCtx.maxLen = 255;
	
	int *varg = (int *) (&format);

	(void)sys_printf(&printCtx, 0, varg);
}



