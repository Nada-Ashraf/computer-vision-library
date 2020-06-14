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
    // https://github.com/raytroop/CSE455-Homework/blob/master/vision-hw1/src/resize_image.c

    // get the coordinates of the box surrounding the interpolated value q
    int left = floorf(x);
    int right = ceilf(x);
    int top = floorf(y);
    int bottom = ceilf(y);

    // get the pixel value of the 4 nearby pixels
    float v1 = get_pixel(im, left, top, c);
    float v2 = get_pixel(im, right, top, c);
    float v3 = get_pixel(im, left, bottom, c);
    float v4 = get_pixel(im, right, bottom, c);

    // distance between the 4 nearby pixels and the interpolated value q
    float d1 = bottom - y;
    float d2 = y - top;
    float d3 = x - left;
    float d4 = right - x;

    // calculate how much each pixel contribute to the interpolated value q
    float q1 = v1 * d1 + v3 * d2;
    float q2 = v2 * d1 + v4 * d2;
    float q = q2 * d3 + q1 * d4;

    return q;
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
