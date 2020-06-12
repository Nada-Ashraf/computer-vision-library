#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int clamp_badding(int value, int min, int max)
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
    x = clamp_badding(x, 0, im.w);
    y = clamp_badding(y, 0, im.h);
    c = clamp_badding(c, 0, im.c);

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
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
}

void clamp_image(image im)
{
    // TODO Fill this in
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
