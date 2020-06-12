#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int clamp_padding(int value, int min, int max)
{
    // if min <= max is TRUE assert() does nothing
    // if FALSE assert() displays an error message on stderr and aborts program execution
    assert(min <= max);

    // if pixel location >= max set it to maximum index
    value = (value >= max) ? max - 1 : value;

    // if pixel location < min value set it to 0
    value = (value < min) ? min : value;

    // return pixel location
    return value;
}

float get_pixel(image im, int x, int y, int c)
{
    // clamp padding
    x = clamp_padding(x, 0, im.w);
    y = clamp_padding(y, 0, im.h);
    c = clamp_padding(c, 0, im.c);

    // in data array the image is stored in CHW format
    // to access a pixel at a certain position:
    // pixel_position(c, y, x) = c * HW + y * W + x
    // https://oneapi-src.github.io/oneDNN/understanding_memory_formats.html
    int index = x + (im.w * y) + (im.w * im.h * c);
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int index = x + (im.w * y) + (im.w * im.h * c);
    im.data[index] = v;
}

image copy_image(image im)
{
    // return an empty image
    image copy = make_image(im.w, im.h, im.c);

    // loop over every channel, width, height index to get all pixels
    for (int c = 0; c < im.c; c++)
    {
        for (int w = 0; w < im.w; w++)
        {
            for (int h = 0; h < im.h; h++)
            {
                // get the pixel value of the current position
                float pixel_value = get_pixel(im, w, h, c);

                // set the pixel value in image copy
                set_pixel(copy, w, h, c, pixel_value);
            }
        }
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    // make an empty image with 1 channel
    image gray = make_image(im.w, im.h, 1);

    // loop over every pixel and set RGB values to greyscale
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            // get R values of the pixel from channel 0
            float R = get_pixel(im, x, y, 0);
            // get G values of the pixel from channel 1
            float G = get_pixel(im, x, y, 1);
            // get B values of the pixel from channel 2
            float B = get_pixel(im, x, y, 2);
            // use luma claculation to find an approximation of perceptual intensity
            float luma_gray = 0.299 * R + 0.587 * G + 0.114 * B;
            // set pixel value to gray
            set_pixel(gray, x, y, 0, luma_gray);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // loop over every pixel
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            // get and set value of pixel
            float pixel_value = get_pixel(im, x, y, c);
            set_pixel(im, x, y, c, pixel_value + v);
        }
    }
}

void clamp_image(image im)
{
    // to avoid overflow
    // make sure the pixel values in the image stay between 0 and 1.
    for (int c = 0; c < im.c; c++)
    {
        for (int x = 0; x < im.w; x++)
        {
            for (int y = 0; y < im.h; y++)
            {
                // get and set value of pixel
                float pixel_value = get_pixel(im, x, y, c);

                // any value below zero gets set to zero
                // and any value above 1 gets set to one.
                float clipped_value = pixel_value > 1.0 ? 1.0 : pixel_value;

                set_pixel(im, x, y, c, clipped_value);
            }
        }
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
}
