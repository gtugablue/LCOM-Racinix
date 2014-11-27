#include "bitmap.h"
#include "video_gr.h"

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
	if (bmp->bitmap_information_header.width % 2 == 0)
	{
		fread(bmp->pixel_array, bmp->bitmap_information_header.image_size, 1, fp);
	}
	else
	{
		// Fix for 16-bit mode
		size_t i;
		uint16_t discard;
		for (i = 0; i < bmp->bitmap_information_header.height; ++i)
		{
			fread((uint16_t *)bmp->pixel_array + i * bmp->bitmap_information_header.width, bmp->bitmap_information_header.width * bmp->bitmap_information_header.bits_per_pixel / 8, 1, fp);
			fread(&discard, 2, 1, fp);
		}
	}


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
	uint16_t *double_buffer = vg_get_double_buffer();
	vbe_mode_info_t *vbe_mode_info = vg_get_vbe_mode_info();
	size_t i, j;
	for (i = x; i < x + bitmap->bitmap_information_header.width; ++i)
	{
		for (j = y; j < y + bitmap->bitmap_information_header.height; ++j)
		{
			vg_set_pixel(i, 2 * y + bitmap->bitmap_information_header.height - j, *((uint16_t *)bitmap->pixel_array + (i - x) + (j - y) * bitmap->bitmap_information_header.width));
		}
	}
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
