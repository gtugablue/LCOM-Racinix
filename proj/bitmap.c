#include "bitmap.h"

#define BITMAP_SIGNATURE		0x4D42

bitmap_t *bitmap_load(const char* filename)
{
	bitmap_t *bmp;
	if ((bmp = (bitmap_t *)malloc(sizeof(bitmap_t))) == NULL)
	{
		return NULL;
	}
	// open file
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == NULL) // "read + binary" mode
	{
		bitmap_delete(bmp);
		return NULL;
	}
	// read the bitmap file header
	bitmap_file_header_t bitmap_file_header;
	fread(&bitmap_file_header, sizeof(bitmap_file_header_t), 1, fp);

	if (bitmap_file_header.signature != BITMAP_SIGNATURE)
	{
		bitmap_delete(bmp);
		return NULL;
	}

	// read the bitmap information header
	fread(&bmp->bitmap_information_header, sizeof(bitmap_information_header_t), 1, fp);

	/*// Support all number of bits per pixel
	if (bmp->bitmap_information_header.bits_per_pixel != 16)
	{
		bitmap_delete(bmp);
		return NULL;
	}*/

	// move file pointer to the beggining of the bitmap data
	fseek(fp, (uint32_t)bitmap_file_header.pixel_array, SEEK_SET);

	if ((bmp->pixel_array = (void *)malloc(bmp->bitmap_information_header.image_size)) == NULL)
	{
		bitmap_delete(bmp);
		return NULL;
	}
	// read the bitmap image data
	fread(bmp->pixel_array, bmp->bitmap_information_header.image_size, 1, fp);

	if (bmp->pixel_array == NULL)
	{
		bitmap_delete(bmp);
		return NULL;
	}

	// Close file
	fclose(fp);

	return bmp;
}

void bitmap_draw(bitmap_t *bitmap, int x, int y)
{
	/*size_t i, j;
	for (i = x; i < x + bitmap->bitmap_information_header.width; ++i)
	{
		for (j = y; j < y + bitmap->bitmap_information_header.height; ++j)
		{
			vg_set_pixel(i, 2 * y + bitmap->bitmap_information_header.height - j, *((uint16_t *)bitmap->pixel_array + (i - x) + (j - y) * bitmap->bitmap_information_header.width));
		}
	}*/
}

void bitmap_delete(bitmap_t *bitmap)
{
	if (bitmap != NULL)
	{
		if (bitmap->pixel_array != NULL)
		{
			free(bitmap->pixel_array);
		}
		free(bitmap);
	}
}
