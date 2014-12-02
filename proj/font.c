#include "font.h"
#include "minix/syslib.h"

font_t *font_load(char* folder)
{
	font_t *font;
	if ((font = malloc(sizeof(font_t))) == NULL)
	{
		return NULL;
	}



	return font;
}

void font_delete(font_t *font)
{
	free(font);
}
