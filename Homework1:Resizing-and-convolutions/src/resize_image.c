#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // this function takes any float pixel location
    // and returns it's the location rounded to the closest int
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    // create new image
    image resized_im = make_image(w, h, im.c);

    // match up cooardinates
    // given two points (-0.5, -0.5) & (new_w, new_h)
    // find aX + b = Y coeffiecents

    // x coeffiecents
    float ax = (float)im.w / w;
    float bx = 0.5 * ax - 0.5;
    // y coeffiecents
    float ay = (float)im.h / h;
    float by = 0.5 * ay - 0.5;

    // iterate over new points and map to old coords
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int c = 0; c < im.c; c++)
            {
                float old_x = x * ax + bx;
                float old_y = y * ay + by;
                float interpolated_value = nn_interpolate(im, old_x, old_y, c);
                set_pixel(resized_im, x, y, c, interpolated_value);
            }
        }
    }

    return resized_im;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // https://github.com/mikeedi/CV-course/blob/master/vision-hw1/src/resize_image.c
    int floor_x = floor(x);
    int ceil_x = ceil(x);

    int floor_y = floor(y);
    int ceil_y = ceil(y);

    float q1 = (y - floor_y) * get_pixel(im, floor_x, ceil_y, c) + (ceil_y - y) * get_pixel(im, floor_x, floor_y, c);
    float q2 = (y - floor_y) * get_pixel(im, ceil_x, ceil_y, c) + (ceil_y - y) * get_pixel(im, ceil_x, floor_y, c);

    float value = (ceil_x - x) * q1 + (x - floor_x) * q2;
    return value;
}

image bilinear_resize(image im, int w, int h)
{
    // create new image
    image resized_im = make_image(w, h, im.c);

    // match up cooardinates
    // given two points (-0.5, -0.5) & (new_w, new_h)
    // find aX + b = Y coeffiecents

    // x coeffiecents
    float ax = (float)im.w / w;
    float bx = 0.5 * ax - 0.5;
    // y coeffiecents
    float ay = (float)im.h / h;
    float by = 0.5 * ay - 0.5;

    // iterate over new points and map to old coords
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int c = 0; c < im.c; c++)
            {
                float old_x = x * ax + bx;
                float old_y = y * ay + by;
                float interpolated_value = bilinear_interpolate(im, old_x, old_y, c);
                set_pixel(resized_im, x, y, c, interpolated_value);
            }
        }
    }

    return resized_im;
}
