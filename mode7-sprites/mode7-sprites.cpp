// mode7-sprites.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CImg.h"

using namespace cimg_library;

static CImg<unsigned char> src;

int get_colour_from_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	return (r ? 1 : 0) + (g ? 2 : 0) + (b ? 4 : 0);
}

int get_pixel_from_image(int x, int y, int x_offset, int y_offset)
{
	x -= x_offset;
	y -= y_offset;

	if (x < 0 || x >= src._width) return 0;
	if (y < 0 || y >= src._height) return 0;

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

	return (get_pixel_from_image(x, y, x_offset, y_offset) ? 1 : 0)
		+ (get_pixel_from_image(x + 1, y, x_offset, y_offset) ? 2 : 0)
		+ (get_pixel_from_image(x, y + 1, x_offset, y_offset) ? 4 : 0)
		+ (get_pixel_from_image(x + 1, y + 1, x_offset, y_offset) ? 8 : 0)
		+ (get_pixel_from_image(x, y + 2, x_offset, y_offset) ? 16 : 0)
		+ (get_pixel_from_image(x + 1, y + 2, x_offset, y_offset) ? 64 : 0);
}

int main(int argc, char **argv)
{
	cimg_usage("MODE 7 sprite convertor.\n\nUsage : mode7-sprites [options]");
	const char *input_name = cimg_option("-i", (char*)0, "Input filename");
	const bool pad = cimg_option("-pad", false, "Pad input dimensions for offset");
	const char *label = cimg_option("-label", (char*)0, "Label");
	const char *output_name = cimg_option("-o", (char*)0, "Output filename");
	const bool mask = cimg_option("-mask", false, "Add mask data to output");
	const bool linear = cimg_option("-linear", false, "Output linear sprite data 1bpp");

	if (cimg_option("-h", false, 0)) std::exit(0);
	if (input_name == NULL)  std::exit(0);

	FILE *outfile = NULL;

	if (output_name)
	{
		outfile = fopen(output_name, "w");

		if (label == NULL)
		{
			printf("Error: Need label if saving\n");
			std::exit(0);
		}
	}

	src.assign(input_name);

	printf("Image=%d x %d\n", src._width, src._height);

	int pixel_width = src._width;
	if (pad) pixel_width++;
	int char_width = pixel_width / 2;
	if (pixel_width % 2) char_width++;

	int pixel_height = src._height;
	if (pad) pixel_height += 2;
	int char_height = pixel_height / 3;
	if (pixel_height % 3) char_height += 3 - (pixel_height % 3);

	printf("Pixels=%d x %d\n", pixel_width, pixel_height);
	printf("Chars=%d x %d\n", char_width, char_height);
	
	if (outfile)
	{
		fprintf(outfile, "\\\\ Input file '%s'\n", input_name);
		fprintf(outfile, "\\\\ Image size=%dx%d pixels=%dx%d\n", src._width, src._height, pixel_width, pixel_height);
		fprintf(outfile, ".%s\n", label);
	}

	
	if (linear)
	{
		int byte_width = char_width / 4;
		int byte_height = pixel_height;

		if (outfile)
		{
			fprintf(outfile, "EQUB %d, %d\t; byte width, char height\n", byte_width, char_height-1);
			fprintf(outfile, ".%s_data\n", label);
		}

		for (int y = 0; y < pixel_height; y++)
		{
			if( outfile )
			{
				fprintf(outfile, "EQUB ");
			}

			for (int xb = 0; xb < byte_width; xb++)
			{
				int x = xb * 8;
				unsigned char linear_byte;

				// Have to reverse the order of bits for each pixel pair because of teletext character codes :S

				linear_byte = (get_pixel_from_image(x, y, 0, 0) == 7 ? 64 : 0)
					+ (get_pixel_from_image(x + 1, y, 0, 0) == 7 ? 128 : 0)
					+ (get_pixel_from_image(x + 2, y, 0, 0) == 7 ? 16 : 0)
					+ (get_pixel_from_image(x + 3, y, 0, 0) == 7 ? 32 : 0)
					+ (get_pixel_from_image(x + 4, y, 0, 0) == 7 ? 4 : 0)
					+ (get_pixel_from_image(x + 5, y, 0, 0) == 7 ? 8 : 0)
					+ (get_pixel_from_image(x + 6, y, 0, 0) == 7 ? 1 : 0)
					+ (get_pixel_from_image(x + 7, y, 0, 0) == 7 ? 2 : 0);

				if (outfile)
				{
					if (xb != 0)
					{
						fprintf(outfile, ",");
					}

					fprintf(outfile, "%d", linear_byte);
				}
			}
			if (outfile)
			{
				fprintf(outfile, "\n");
			}
		}
	}
	else
	{
		if (outfile)
		{
			fprintf(outfile, "EQUB %d, %d\t;char width, char height\n", char_width, char_height);
			fprintf(outfile, ".%s_table\n", label);

			for (int x_offset = 0; x_offset < 2; x_offset++)
			{
				for (int y_offset = 0; y_offset < 3; y_offset++)
				{
					fprintf(outfile, "EQUB LO(%s_char%d%d), HI(%s_char%d%d)\n", label, x_offset, y_offset, label, x_offset, y_offset);
				}
			}

			if (mask)
			{
				fprintf(outfile, ".%s_masks\n", label);

				for (int x_offset = 0; x_offset < 2; x_offset++)
				{
					for (int y_offset = 0; y_offset < 3; y_offset++)
					{
						fprintf(outfile, "EQUB LO(%s_mask%d%d), HI(%s_mask%d%d)\n", label, x_offset, y_offset, label, x_offset, y_offset);
					}
				}
			}
		}

		for (int x_offset = 0; x_offset < 2; x_offset++)
		{
			for (int y_offset = 0; y_offset < 3; y_offset++)
			{
				printf("Offset [%d, %d]\n", x_offset, y_offset);

				if (outfile)
				{
					fprintf(outfile, ".%s_char%d%d\n", label, x_offset, y_offset);
				}

				for (int y7 = 0; y7 < char_height; y7++)
				{
					if (outfile)
					{
						fprintf(outfile, "EQUB ");
					}

					for (int x7 = 0; x7 < char_width; x7++)
					{
						unsigned char gfx_char = get_graphic_char_from_image(x7, y7, x_offset, y_offset);

						if (outfile)
						{
							if (x7 != 0)
							{
								fprintf(outfile, ",");
							}
							fprintf(outfile, "%d", gfx_char);		// , mask_char);
						}
					}

					if (outfile)
					{
						fprintf(outfile, "\n");
					}
				}
			}
		}

		for (int x_offset = 0; x_offset < 2; x_offset++)
		{
			for (int y_offset = 0; y_offset < 3; y_offset++)
			{
				printf("Offset [%d, %d]\n", x_offset, y_offset);

				if (outfile)
				{
					fprintf(outfile, ".%s_mask%d%d\n", label, x_offset, y_offset);
				}

				for (int y7 = 0; y7 < char_height; y7++)
				{
					if (outfile)
					{
						fprintf(outfile, "EQUB ");
					}

					for (int x7 = 0; x7 < char_width; x7++)
					{
						unsigned char mask_char = get_mask_char_from_image(x7, y7, x_offset, y_offset);

						if (outfile)
						{
							if (x7 != 0)
							{
								fprintf(outfile, ",");
							}
							fprintf(outfile, "%d", mask_char);		// , mask_char);
						}
					}

					if (outfile)
					{
						fprintf(outfile, "\n");
					}
				}
			}
		}

	}

	if (outfile)
	{
		fclose(outfile);
		outfile = NULL;
	}

	return 0;
}

