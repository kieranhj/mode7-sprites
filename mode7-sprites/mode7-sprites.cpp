// mode7-sprites.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CImg.h"

using namespace cimg_library;

static CImg<unsigned char> src;

static int global_x = 0;
static int global_y = 0;
static int global_w = 0;
static int global_h = 0;

int get_colour_from_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	int col = (r ? 1 : 0) + (g ? 2 : 0) + (b ? 4 : 0);
//	int col = r ? 7 : (g ? 7 : (b ? 7 : 0));

	return col;
}

int get_pixel_from_image(int x, int y, int x_offset, int y_offset)
{
	x -= x_offset;
	y -= y_offset;

	if (x < 0 || x >= global_w) return 0;
	if (y < 0 || y >= global_h) return 0;

	x += global_x;
	y += global_y;

	// printf("(%d, %d) = [%d %d %d] depth=%d\n", x, y, src(x, y, 0), src(x, y, 1), src(x, y, 2), src._depth);

	return get_colour_from_rgb(src(x, y, 0), src(x, y, 1), src(x, y, 2));
}

unsigned char get_graphic_char_from_image(int x7, int y7, int x_offset, int y_offset)
{
	int x = x7 * 2;
	int y = y7 * 3;

	// This is our default graphics character
	// All pixels matching background are background
	// All other pixels are foreground
	// NB. this only works with -quant

	return 32
		+ (get_pixel_from_image(x, y, x_offset, y_offset) == 7 ? 1 : 0)
		+ (get_pixel_from_image(x + 1, y, x_offset, y_offset) == 7 ? 2 : 0)
		+ (get_pixel_from_image(x, y + 1, x_offset, y_offset) == 7 ? 4 : 0)
		+ (get_pixel_from_image(x + 1, y + 1, x_offset, y_offset) == 7 ? 8 : 0)
		+ (get_pixel_from_image(x, y + 2, x_offset, y_offset) == 7 ? 16 : 0)
		+ (get_pixel_from_image(x + 1, y + 2, x_offset, y_offset) == 7 ? 64 : 0);
}

unsigned char get_mask_char_from_image(int x7, int y7, int x_offset, int y_offset)
{
	int x = x7 * 2;
	int y = y7 * 3;

	// Anything that is not black is considered mask

	unsigned char mask = (get_pixel_from_image(x, y, x_offset, y_offset) ? 1 : 0)
		+ (get_pixel_from_image(x + 1, y, x_offset, y_offset) ? 2 : 0)
		+ (get_pixel_from_image(x, y + 1, x_offset, y_offset) ? 4 : 0)
		+ (get_pixel_from_image(x + 1, y + 1, x_offset, y_offset) ? 8 : 0)
		+ (get_pixel_from_image(x, y + 2, x_offset, y_offset) ? 16 : 0)
		+ (get_pixel_from_image(x + 1, y + 2, x_offset, y_offset) ? 64 : 0);

	return mask ^ 0xff;
}

void make_sprite_data(FILE *outfile, const char *label, int pixel_width, int pixel_height, bool swizzle, bool mask)
{
	fprintf(outfile, ".%s_data\n", label);

	if (swizzle)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			fprintf(outfile, "EQUB ");

			for (int y = -2; y < pixel_height - 2; y += 3)
			{
				if (y != -2)
				{
					fprintf(outfile, ",");
				}

				unsigned char pixels = (get_pixel_from_image(x, y, 0, 0) == 7 ? 16 : 0)
					+ (get_pixel_from_image(x, y + 1, 0, 0) == 7 ? 8 : 0)
					+ (get_pixel_from_image(x, y + 2, 0, 0) == 7 ? 4 : 0)
					+ (get_pixel_from_image(x, y + 3, 0, 0) == 7 ? 2 : 0)
					+ (get_pixel_from_image(x, y + 4, 0, 0) == 7 ? 1 : 0);

				fprintf(outfile, "%d", pixels);
			}

			fprintf(outfile, "\n");
		}
	}
	else
	{
		for (int y = -2; y < pixel_height - 2; y += 3)
		{
			fprintf(outfile, "EQUB ");

			for (int x = 0; x < pixel_width; x++)
			{
				if (x != 0)
				{
					fprintf(outfile, ",");
				}

				unsigned char pixels = (get_pixel_from_image(x, y, 0, 0) == 7 ? 16 : 0)
					+ (get_pixel_from_image(x, y + 1, 0, 0) == 7 ? 8 : 0)
					+ (get_pixel_from_image(x, y + 2, 0, 0) == 7 ? 4 : 0)
					+ (get_pixel_from_image(x, y + 3, 0, 0) == 7 ? 2 : 0)
					+ (get_pixel_from_image(x, y + 4, 0, 0) == 7 ? 1 : 0);

				fprintf(outfile, "%d", pixels);
			}

			fprintf(outfile, "\n");
		}
	}

	if (mask)
	{
		fprintf(outfile, ".%s_mask\n", label);

		if (swizzle)
		{
			for (int x = 0; x < pixel_width; x++)
			{
				fprintf(outfile, "EQUB ");

				for (int y = -2; y < pixel_height - 2; y += 3)
				{
					if (y != -2)
					{
						fprintf(outfile, ",");
					}

					unsigned char pixels = (get_pixel_from_image(x, y, 0, 0) == 0 ? 16 : 0)
						+ (get_pixel_from_image(x, y + 1, 0, 0) == 0 ? 8 : 0)
						+ (get_pixel_from_image(x, y + 2, 0, 0) == 0 ? 4 : 0)
						+ (get_pixel_from_image(x, y + 3, 0, 0) == 0 ? 2 : 0)
						+ (get_pixel_from_image(x, y + 4, 0, 0) == 0 ? 1 : 0);

					fprintf(outfile, "%d", pixels);
				}

				fprintf(outfile, "\n");
			}
		}
		else
		{
			for (int y = -2; y < pixel_height - 2; y += 3)
			{
				fprintf(outfile, "EQUB ");

				for (int x = 0; x < pixel_width; x++)
				{
					if (x != 0)
					{
						fprintf(outfile, ",");
					}

					unsigned char pixels = (get_pixel_from_image(x, y, 0, 0) == 0 ? 16 : 0)
						+ (get_pixel_from_image(x, y + 1, 0, 0) == 0 ? 8 : 0)
						+ (get_pixel_from_image(x, y + 2, 0, 0) == 0 ? 4 : 0)
						+ (get_pixel_from_image(x, y + 3, 0, 0) == 0 ? 2 : 0)
						+ (get_pixel_from_image(x, y + 4, 0, 0) == 0 ? 1 : 0);

					fprintf(outfile, "%d", pixels);
				}

				fprintf(outfile, "\n");
			}
		}
	}
}

void make_six_data(FILE *outfile, const char *label, int char_width, int char_height, int align, bool mask)
{
#if 0
	if (!align)
	{
		fprintf(outfile, ".%s_table\n", label);

		fprintf(outfile, ".%s_table_LO\n", label);
		for (int y_offset = 0; y_offset < 3; y_offset++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, "EQUB LO(%s_data_%d%d)\n", label, x_offset, y_offset);
			}
		}
		fprintf(outfile, ".%s_table_HI\n", label);
		for (int y_offset = 0; y_offset < 3; y_offset++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, "EQUB HI(%s_data_%d%d)\n", label, x_offset, y_offset);
			}
		}
		fprintf(outfile, "\\\\ Corresponding mask data is %d bytes following sprite data\n", char_width * char_height * 6);
	}
#endif

	fprintf(outfile, ".%s_data\n", label);

	for (int y_offset = 0; y_offset < 3; y_offset++)
	{
		for (int x_offset = 0; x_offset < 2; x_offset++)
		{
			fprintf(outfile, ".%s_data_%d%d\t; x_offset=%d, y_offset=%d\n", label, x_offset, y_offset, x_offset, y_offset);

			for (int y7 = 0; y7 < char_height; y7++)
			{
				fprintf(outfile, "EQUB ");
				for (int x7 = 0; x7 < char_width; x7++)
				{
					if (x7 != 0)
					{
						fprintf(outfile, ",");
					}
					fprintf(outfile, "%d", get_graphic_char_from_image(x7, y7, x_offset, y_offset));
				}
				fprintf(outfile, "\n");
			}

			if (align)
			{
				fprintf(outfile, "ALIGN %d\n", align);
			}
		}
	}

	if (mask)
	{
		fprintf(outfile, ".%s_mask\n", label);

		for (int y_offset = 0; y_offset < 3; y_offset++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, ".%s_mask_%d%d\t; x_offset=%d, y_offset=%d\n", label, x_offset, y_offset, x_offset, y_offset);

				for (int y7 = 0; y7 < char_height; y7++)
				{
					fprintf(outfile, "EQUB ");
					for (int x7 = 0; x7 < char_width; x7++)
					{
						if (x7 != 0)
						{
							fprintf(outfile, ",");
						}
						fprintf(outfile, "%d", get_mask_char_from_image(x7, y7, x_offset, y_offset));
					}
					fprintf(outfile, "\n");
				}

				if (align)
				{
					fprintf(outfile, "ALIGN %d\n", align);
				}
			}
		}
	}
}

void make_horiz_data(FILE *outfile, const char *label, int char_width, int char_height, int align, bool mask)
{
#if 0
	if (!align)
	{
		fprintf(outfile, ".%s_table\n", label);

		fprintf(outfile, ".%s_table_LO\n", label);
		for (int y_offset = 0; y_offset < 3; y_offset++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, "EQUB LO(%s_data_%d%d)\n", label, x_offset, y_offset);
			}
		}
		fprintf(outfile, ".%s_table_HI\n", label);
		for (int y_offset = 0; y_offset < 3; y_offset++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, "EQUB HI(%s_data_%d%d)\n", label, x_offset, y_offset);
			}
		}
		fprintf(outfile, "\\\\ Corresponding mask data is %d bytes following sprite data\n", char_width * char_height * 6);
	}
#endif

	fprintf(outfile, ".%s_data\n", label);

	for (int y_row = 0; y_row < 3; y_row++)
	{
		for (int x_offset = 0; x_offset < 2; x_offset++)
		{
			fprintf(outfile, ".%s_data_%d%d\t; x_offset=%d, y_row=%d\n", label, x_offset, y_row, x_offset, y_row);

			for (int y7 = 0; y7 < char_height; y7++)
			{
				fprintf(outfile, "EQUB ");
				for (int x7 = 0; x7 < char_width; x7++)
				{
					if (x7 != 0)
					{
						fprintf(outfile, ",");
					}

					unsigned char gfx = get_graphic_char_from_image(x7, y7, x_offset, 0);

					switch (y_row)
					{
					case 0:
						gfx &= (1 | 2);
						break;

					case 1:
						gfx &= (4 | 8);
						break;

					case 2:
						gfx &= (16 | 64);
						break;

					default:
						break;
					}

					fprintf(outfile, "%d", gfx);
				}
				fprintf(outfile, "\n");
			}

			if (align)
			{
				fprintf(outfile, "ALIGN %d\n", align);
			}
		}
	}

	if (mask)
	{
		fprintf(outfile, ".%s_mask\n", label);

		for (int y_row = 0; y_row < 3; y_row++)
		{
			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				fprintf(outfile, ".%s_mask_%d%d\t; x_offset=%d, y_row=%d\n", label, x_offset, y_row, x_offset, y_row);

				for (int y7 = 0; y7 < char_height; y7++)
				{
					fprintf(outfile, "EQUB ");
					for (int x7 = 0; x7 < char_width; x7++)
					{
						if (x7 != 0)
						{
							fprintf(outfile, ",");
						}

						unsigned char gfx = get_mask_char_from_image(x7, y7, x_offset, 0);

						switch (y_row)
						{
						case 0:
							gfx &= (1 | 2);
							break;

						case 1:
							gfx &= (4 | 8);
							break;

						case 2:
							gfx &= (16 | 64);
							break;

						default:
							break;
						}

						fprintf(outfile, "%d", gfx);
					}

					fprintf(outfile, "\n");
				}

				if (align)
				{
					fprintf(outfile, "ALIGN %d\n", align);
				}
			}
		}
	}
}


int main(int argc, char **argv)
{
	cimg_usage("MODE 7 sprite convertor.\n\nUsage : mode7-sprites [options]");
	const char *input_name = cimg_option("-i", (char*)0, "Input filename");
	const bool pad = cimg_option("-pad", false, "Pad input dimensions for offset");
	const char *label = cimg_option("-label", (char*)0, "Label");
	const char *output_name = cimg_option("-o", (char*)0, "Output filename");
	const bool mask = cimg_option("-mask", false, "Add mask data to output");
	const bool font = cimg_option("-font", false, "Extract font glyphs / sprite sheet");
	const char *const geom = cimg_option("-g", "16x16", "Sprite input size (when extracting from font/sprite sheet)");
	const int num_glyphs = cimg_option("-n", 0, "Num glyphs / sprites to extract (0=all)");
	const bool swizzle = cimg_option("-swizzle", false, "Output data in column order (default=row order)");
	const bool six = cimg_option("-six", false, "Generate six pre-shifted offsets as sprite data");
	const bool horiz = cimg_option("-horiz", false, "Generate horizontal strips as sprite data");
	const int align = cimg_option("-align", 0, "Align sprite data blocks");

	if (cimg_option("-h", false, 0)) std::exit(0);
	if (input_name == NULL)  std::exit(0);
	if (output_name == NULL)  std::exit(0);
	if (label == NULL)  std::exit(0);

	FILE *outfile = fopen(output_name, "w");

	if (outfile == NULL)
	{
		printf("Failed to open output file '%s' for writing.\n", output_name);
		std::exit(0);
	}

	src.assign(input_name);

	printf("Image=%d x %d\n", src._width, src._height);

	int pixel_width = src._width;
	int pixel_height = src._height;

	if (font)
	{
		std::sscanf(geom, "%d%*c%d", &pixel_width, &pixel_height);
	}
	else
	{
		if (pad) pixel_width++;
		if (pad) pixel_height += 2;
	}

	global_w = pixel_width;
	global_h = pixel_height;

	int char_width = pixel_width / 2;
	if (pixel_width % 2) char_width++;
	int char_height = pixel_height / 3;
	if (pixel_height % 3) char_height++;

	printf("Pixels=%d x %d\n", pixel_width, pixel_height);
	printf("Chars=%d x %d\n", char_width, char_height);

	fprintf(outfile, "\\\\ Input file '%s'\n", input_name);
	fprintf(outfile, "\\\\ Image size=%dx%d pixels=%dx%d\n", src._width, src._height, pixel_width, pixel_height);
	fprintf(outfile, ".%s\n", label);

	if (six)
	{
		fprintf(outfile, "EQUB %d, %d\t;char width, char height\n", char_width, char_height);

		if (align)
		{
			fprintf(outfile, "ALIGN %d\n", align);
		}
	}
	else
	{
		fprintf(outfile, "\\\\ Data in %s order\n", swizzle ? "COLUMN" : "ROW");

		if (swizzle)
		{
			fprintf(outfile, "EQUB %d, %d\t;char height, pixel width\n", char_height, pixel_width);
		}
		else
		{
			fprintf(outfile, "EQUB %d, %d\t;pixel width, char height\n", pixel_width, char_height);
		}
	}

	if (font)
	{
		int max = (src._height / pixel_height)*(src._width / pixel_width);

		if (num_glyphs && max > num_glyphs)
		{
			max = num_glyphs;
		}

		fprintf(outfile, ".%s_table_LO\n", label);
		for (int i = 0; i < max; i++)
		{
			fprintf(outfile, "EQUB LO(%s%d_data)\n", label, i);
		}
		fprintf(outfile, ".%s_table_HI\n", label);
		for (int i = 0; i < max; i++)
		{
			fprintf(outfile, "EQUB HI(%s%d_data)\n", label, i);
		}

		int n = 0;
		for (int gy = 0; gy < src._height; gy += pixel_height)
		{
			for (int gx = 0; gx < src._width && n < max; gx += pixel_width, n++)
			{
				global_x = gx;
				global_y = gy;

				char gl[256];
				sprintf(gl, "%s%d", label, n);

				printf("%d at (%d, %d)\n", n, gx, gy);

				make_sprite_data(outfile, gl, pixel_width, pixel_height, swizzle, mask);
			}
		}
	}
	else
	{
		if (six)
		{
			make_six_data(outfile, label, char_width, char_height, align, mask);
		}
		else if (horiz)
		{
			make_horiz_data(outfile, label, char_width, char_height, align, mask);
		}
		else
		{
			make_sprite_data(outfile, label, pixel_width, pixel_height, swizzle, mask);
		}
	}

	fclose(outfile);

	return 0;
}
