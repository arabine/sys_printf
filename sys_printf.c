/**
BSD 3-Clause License

Copyright (c) 2018, Anthony Rabine
All rights reserved.
*/
#include "sys_printf.h"
#include <stdbool.h>

static const uint8_t PAD_RIGHT = 1U;
static const uint8_t PAD_ZERO = 2U;
static const uint8_t PRINT_BUF_LEN = 12U; // the following should be enough for 32 bit int

// ******************************************************************************
// *                               STATIC MEMBERS                               *
// ******************************************************************************

void sys_printchar (sys_print_ctx_t *ctx, char **str, char c)
{
	if (str)
	{
		bool enable = true;
		if (ctx->maxLen > 0U)
		{
			ctx->len++;
			if (ctx->len >= ctx->maxLen)
			{
				enable = false;
			}
		}

		if (enable)
		{
			**str = c;
			++(*str);
		}
	}
	else
	{
		if (ctx->pPutc != NULL)
		{
			(void) ctx->pPutc(c);
		}
	}
}

static int sys_prints (sys_print_ctx_t *ctx, char **out, const char *string, int width, int pad)
{
	int pc = 0;
    char padchar = ' ';
    
	if (width > 0)
	{
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr)
		{
			++len;
		}
		if (len >= width)
		{
			width = 0;
		}
		else
		{
			width -= len;
		}
		if (pad & PAD_ZERO)
		{
			padchar = '0';
		}
	}
	if (!(pad & PAD_RIGHT))
	{
		for (; width > 0; --width)
		{
			sys_printchar(ctx, out, padchar);
			++pc;
		}
	}
	for (; *string; ++string)
	{
		sys_printchar(ctx, out, *string);
		++pc;
	}
	for (; width > 0; --width)
	{
		sys_printchar(ctx, out, padchar);
		++pc;
	}
	return pc;
}

static int sys_printi (sys_print_ctx_t *ctx, char **out, int32_t i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	char *s;
	int32_t t, neg = 0, pc = 0;
	uint32_t u = (uint32_t) i;

	if (i == 0)
	{
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return sys_prints(ctx, out, print_buf, width, pad);
	}
	if (sg && b == 10 && i < 0)
	{
		neg = 1;
		u = (uint32_t) - i;
	}
	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';
	while (u)
	{
		t = u % b;  //lint !e573  Warning 573: Signed-unsigned mix with divide
		if (t >= 10)
		{
			t += letbase - '0' - 10;
		}
		*--s = t + '0';
		u /= b;  //lint !e573  Warning 573: Signed-unsigned mix with divide
	}
	if (neg)
	{
		if (width && (pad & PAD_ZERO))
		{
			sys_printchar(ctx, out, '-');
			++pc;
			--width;
		}
		else
		{
			*--s = '-';
		}
	}
	return pc + sys_prints(ctx, out, s, width, pad);
}

int sys_printf (sys_print_ctx_t *ctx, char **out, const char *fmt, va_list varg)
{
	int width, pad;
	int pc = 0;
	const char *format = fmt;
	int post_decimal;
	unsigned dec_width = 0;
	char scr[2];

	for (; (*format != 0); ++format)
	{
		if (*format == '%')
		{
			++format;
			width = pad = 0;
			if (*format == '\0')
			{
				break;
			}
			if (*format == '%')
			{
				goto out;
			}
			if (*format == '-')
			{
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0')
			{
				++format;
				pad |= PAD_ZERO;
			}
			post_decimal = 0;
			if (*format == '.' || (*format >= '0' && *format <= '9'))
			{

				while (1)
				{
					if (*format == '.')
					{
						post_decimal = 1;
						dec_width = 0;
						format++;
					}
					else if ((*format >= '0' && *format <= '9'))
					{
						if (post_decimal)
						{
							dec_width *= 10;
							dec_width += *format - '0';
						}
						else
						{
							width *= 10;
							width += *format - '0';
						}
						format++;
					}
					else
					{
						break;
					}
				}
			}
			if (*format == 'l')
			{
				++format;
			}
			switch (*format)
			{
				case 's':
				{
					char *s = va_arg(varg, char *);
					// printf("[%s] w=%u\n", s, width) ;
					pc += sys_prints(ctx, out, s ? s : "(null)", width, pad);
				}
				break;
				case 'd':
					pc += sys_printi(ctx, out, va_arg(varg, int32_t), 10, 1, width, pad, 'a');
					break;
				case 'x':
					pc += sys_printi(ctx, out, va_arg(varg, int32_t), 16, 0, width, pad, 'a');
					break;
				case 'X':
					pc += sys_printi(ctx, out, va_arg(varg, int32_t), 16, 0, width, pad, 'A');
					break;
				case 'u':
					pc += sys_printi(ctx, out, va_arg(varg, int32_t), 10, 0, width, pad, 'a');
					break;
				case 'c':
					/* char are converted to int then pushed on the stack */
					scr[0] = va_arg(varg, int32_t);
					scr[1] = '\0';
					pc += sys_prints(ctx, out, scr, width, pad);
					break;

				default:
					sys_printchar(ctx, out, '%');
					sys_printchar(ctx, out, *format);
					break;
			}
		}
		else
		{
out:
sys_printchar(ctx, out, *format);
			++pc;
		}
	}

	sys_printchar(ctx, out, '\0'); // ensure null-terminated

	return pc;
}




