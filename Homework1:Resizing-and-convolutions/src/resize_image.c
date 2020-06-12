#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // this function takes any float pixel location
    // and returns it's the location rounded to the closest int
    int x_rounded = round(x);
    int y_rounded = round(y);
    int pixel_value = get_pixel(im, x_rounded, y_rounded, c);
    return pixel_value;
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    return make_image(1, 1, 1);
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    return 0;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    return make_image(1, 1, 1);
}
