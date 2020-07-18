/*

Copyright (c)  2020  Igor van den Hoven  ivdhoven@gmail.com

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/

/*
	Compile using: gcc <filename>
*/

/*
	I didn't feel like including an object handler, so instead I will
	include two example functions to convert json data to vton data and
	vice versa.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 10000

#define VTON_VARIABLE    249
#define VTON_VALUE       250
#define VTON_TABLE_OPEN  251
#define VTON_TABLE_CLOSE 252
#define VTON_ARRAY_OPEN  253
#define VTON_ARRAY_CLOSE 254

/*
	VTON utility functions
*/

int vton2json(unsigned char *src, int srclen, char *out)
{
	char *pto;
	int i, nest, last;

	nest = i = 0;

	pto = out;

	last = VTON_VARIABLE;

	while (i < srclen && nest >= 0)
	{
		if (src[i] == 0)
		{
			break;
		}

		switch (src[i])
		{
			case VTON_TABLE_OPEN:
				nest++;
				*pto++ = '{';
				last = VTON_TABLE_OPEN;
				break;

			case VTON_TABLE_CLOSE:
				if (last == VTON_VALUE || last == VTON_VARIABLE)
				{
					*pto++ = '"';
				}
				nest--;
				*pto++ = '}';
				last = VTON_TABLE_CLOSE;
				break;

			case VTON_ARRAY_OPEN:
				nest++;
				*pto++ = '[';
				last = VTON_ARRAY_OPEN;
				break;

			case VTON_ARRAY_CLOSE:
				if (last == VTON_VALUE || last == VTON_VARIABLE)
				{
					*pto++ = '"';
				}
				nest--;
				*pto++ = ']';
				last = VTON_ARRAY_CLOSE;
				break;

			case VTON_VARIABLE:
				if (last == VTON_VALUE || last == VTON_VARIABLE)
				{
					*pto++ = '"';
				}

				if (last == VTON_VALUE || last == VTON_VARIABLE || last == VTON_TABLE_CLOSE || last == VTON_ARRAY_CLOSE)
				{
					*pto++ = ',';
					*pto++ = ' ';
				}
				*pto++ = '"';
				last = VTON_VARIABLE;
				break;

			case VTON_VALUE:
				if (last == VTON_VARIABLE)
				{
					*pto++ = '"';
					*pto++ = ':';
				}

				if (last == VTON_VALUE)
				{
					*pto++ = '"';
					*pto++ = ',';
					*pto++ = ' ';
				}

				if (last == VTON_TABLE_CLOSE || last == VTON_ARRAY_CLOSE)
				{
					*pto++ = ',';
					*pto++ = ' ';
				}

				if (src[i+1] != VTON_TABLE_OPEN && src[i+1] != VTON_ARRAY_OPEN)
				{
					*pto++ = '"';
				}
				last = VTON_VALUE;
				break;

			case '\\':
				*pto++ = '\\';
				*pto++ = '\\';
				break;

			case '"':
				*pto++ = '\\';
				*pto++ = '"';
				break;

			default:
				*pto++ = src[i];
				break;
		}
		i++;
	}
	*pto++ = 0;

	return pto - out;
}

int json2vton(unsigned char *src, int srclen, char *out)
{
	char *pto;
	int i, nest, last, type, state[100];

	nest = i = 0;

	pto = out;

	state[0] = nest = type = 0;

	last = 1;

	while (i < srclen && src[i] != 0 && nest < 99)
	{
		switch (src[i])
		{
			case ' ':
			case '\t':
				i++;
				break;

			case '{':
				*pto++ = state[++nest] = last = VTON_TABLE_OPEN;
				i++;
				break;

			case '}':
				nest--;
				i++;
				*pto++ = last = VTON_TABLE_CLOSE;
				break;

			case '[':
				i++;
				*pto++ = state[++nest] = last = VTON_ARRAY_OPEN;
				break;

			case ']':
				nest--;
				i++;
				*pto++ = last = VTON_ARRAY_CLOSE;
				break;

			case ':':
				*pto++ = last = VTON_VALUE;
				i++;
				break;

			case ',':
				i++;
				if (state[nest] == VTON_ARRAY_OPEN)
				{
					*pto++ = last = VTON_VALUE;
				}
				else
				{
					*pto++ = last = VTON_VARIABLE;
				}
				break;

			case '"':
				i++;
				if (last == 0 || last == VTON_TABLE_OPEN)
				{
					*pto++ = last = VTON_VARIABLE;
				}
				if (last == VTON_ARRAY_OPEN)
				{
					*pto++ = last = VTON_VALUE;
				}

				while (i < srclen && src[i] != 0)
				{
					if (src[i] == '\\')
					{
						i++;
						*pto++ = '\\';

						if (i < srclen && src[i] == '"')
						{
							*pto++ = src[i++];
						}
					}
					else if (src[i] == '"')
					{
						i++;
						break;
					}
					else
					{
						*pto++ = src[i++];
					}
				}
				break;

			default:
				type = 1;

				while (i < srclen && src[i] != 0 && type)
				{
					switch (src[i])
					{
						case '}':
						case ']':
						case ',':
						case ':':
							type = 0;
							break;

						case ' ':
						case '\t':
							i++;
							break;

						default:
							*pto++ = src[i++];
							break;
					}
				}
				break;
		}
	}
	*pto++ = 0;

	return pto - out;
}


/*
	Testing
*/

int main(void)
{
	char input[BUFFER_SIZE], output[BUFFER_SIZE];
	int size;

	strcpy(input, ""
"{"
"	\"id\": \"0001\","
"	\"type\": \"donut\","
"	\"name\": \"Cake\","
"	\"ppu\": 0.55,"
"	\"batters\":"
"		{"
"			\"batter\":"
"				["
"					{ \"id\": \"1001\", \"type\": \"Regular\" },"
"					{ \"id\": \"1002\", \"type\": \"Chocolate\" },"
"					{ \"id\": \"1003\", \"type\": \"Blueberry\" },"
"					{ \"id\": \"1004\", \"type\": \"Devil's Food\" }"
"				]"
"		},"
"	\"topping\":"
"		["
"			{ \"id\": \"5001\", \"type\": \"None\" },"
"			{ \"id\": \"5002\", \"type\": \"Glazed\" },"
"			{ \"id\": \"5005\", \"type\": \"Sugar\" },"
"			{ \"id\": \"5007\", \"type\": \"Powdered Sugar\" },"
"			{ \"id\": \"5006\", \"type\": \"Chocolate with Sprinkles\" },"
"			{ \"id\": \"5003\", \"type\": \"Chocolate\" },"
"			{ \"id\": \"5004\", \"type\": \"Maple\" }"
"		]"
"}"
);

	strcpy(output, "");

	size = json2vton(input, strlen(input), output);

	printf("input is:\n\n%s\n(%d) \n\noutput is:\n\n%s\n\n", input, strlen(input), output);

	memcpy(input, output, size + 1);

	strcpy(output, "");

	vton2json(input, size, output);

	printf("input is:\n\n%s\n(%d)\n\noutput is:\n\n%s\n\n", input, size, output);

	return 0;
}
