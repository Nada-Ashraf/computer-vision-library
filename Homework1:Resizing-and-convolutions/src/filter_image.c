#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for (int c = 0; c < im.c; c++)
    {
        // loop over the image to calculate the summation of pixel values
        float pixel_sum = 0;
        for (int x = 0; x < im.w; x++)
        {
            for (int y = 0; y < im.h; y++)
            {
                float pixel_value = get_pixel(im, x, y, c);
                pixel_sum += pixel_value;
            }
        }
        // loop over the image and normalize each pixel value
        for (int x = 0; x < im.w; x++)
        {
            for (int y = 0; y < im.h; y++)
            {
                float pixel_value = get_pixel(im, x, y, c);
                set_pixel(im, x, y, c, pixel_value / pixel_sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    // make a square image w * w with one channel
    image filter = make_image(w, w, 1);

    // fill the image with uniform entries that sum to 1
    for (int i = 0; i < w * w; i++)
        filter.data[i] = 1.0;
    l1_normalize(filter);

    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // Note that filter's size can'n be even, i.e 1x1, 3x3, 5x5 etc
    // filter better have either the same number of channels as im or have 1 channel
    assert(im.c == filter.c || filter.c == 1);

    // 3 cases:
    // 1. filter and im have the same number of channels
    // 2. If preserve is set to 1 we should produce an image with the same number of channels as the input
    // 3. If the filter only has one channel but im has multiple channels
    int numOfChannel;
    if (im.c == filter.c)
    {
        numOfChannel = 1;
        if (preserve == 1)
        {
            numOfChannel = im.c;
        }
    }
    else if (filter.c == 1)
    {
        numOfChannel = 1;
        if (preserve == 1)
        {
            numOfChannel = 3;
        }
    }
    image convolved = make_image(im.w, im.h, numOfChannel);

    float sum, x_im, y_im, filter_pixel, im_pixel;

    // loop over every pixel in image
    for (int w = 0; w < im.w; w++)
    {
        for (int h = 0; h < im.h; h++)
        {
            sum = 0;
            for (int c = 0; c < im.c; c++)
            {
                // set sum to zero when starting to convolve over different channel
                if (numOfChannel > 1)
                {
                    sum = 0;
                }

                // loop over the filter pixels and apply filter to pixel in the center
                for (int filter_w = 0; filter_w < filter.w; filter_w++)
                {
                    // to apply to pixel in the center of filter
                    x_im = w + filter_w - filter.w / 2;
                    for (int filter_h = 0; filter_h < filter.h; filter_h++)
                    {
                        // to apply to pixel in the center of filter
                        y_im = h + filter_h - filter.h / 2;

                        // get the center pixel value of the image
                        im_pixel = get_pixel(im, x_im, y_im, c);

                        // get the center pixel value of the filter
                        if (filter.c > 1)
                        {
                            filter_pixel = get_pixel(filter, filter_w, filter_h, c);
                        }
                        else
                        {
                            filter_pixel = get_pixel(filter, filter_w, filter_h, 0);
                        }
                        // get the weighted sum over all values resulting from
                        // convulving the certain pixel with the kernel
                        sum += im_pixel * filter_pixel;
                    }
                }
                // set pixel to new value
                if (numOfChannel > 1)
                {
                    set_pixel(convolved, w, h, c, sum);
                }
            }

            if (numOfChannel == 1)
            {
                set_pixel(convolved, w, h, 0, sum);
            }
        }
    }
    return convolved;
}

image make_highpass_filter()
{
    int numOfChannel = 1;
    image highpass = make_image(3, 3, numOfChannel);

    for (int c = 0; c < numOfChannel; c++)
    {
        set_pixel(highpass, 0, 0, c, 0.);
        set_pixel(highpass, 0, 1, c, -1);
        set_pixel(highpass, 0, 2, c, 0.);

        set_pixel(highpass, 1, 0, c, -1.);
        set_pixel(highpass, 1, 1, c, 4.);
        set_pixel(highpass, 1, 2, c, -1.);

        set_pixel(highpass, 2, 0, c, 0.);
        set_pixel(highpass, 2, 1, c, -1.);
        set_pixel(highpass, 2, 2, c, 0.);
    }
    return highpass;
}

image make_sharpen_filter()
{
    int numOfChannel = 1;
    image sharpen = make_image(3, 3, numOfChannel);

    for (int c = 0; c < numOfChannel; c++)
    {
        set_pixel(sharpen, 0, 0, c, 0.);
        set_pixel(sharpen, 0, 1, c, -1);
        set_pixel(sharpen, 0, 2, c, 0.);

        set_pixel(sharpen, 1, 0, c, -1.);
        set_pixel(sharpen, 1, 1, c, 5.);
        set_pixel(sharpen, 1, 2, c, -1.);

        set_pixel(sharpen, 2, 0, c, 0.);
        set_pixel(sharpen, 2, 1, c, -1.);
        set_pixel(sharpen, 2, 2, c, 0.);
    }
    return sharpen;
}

image make_emboss_filter()
{
    int numOfChannel = 1;
    image emboss = make_image(3, 3, numOfChannel);

    for (int c = 0; c < numOfChannel; c++)
    {
        set_pixel(emboss, 0, 0, c, -2.);
        set_pixel(emboss, 0, 1, c, -1);
        set_pixel(emboss, 0, 2, c, 0.);

        set_pixel(emboss, 1, 0, c, -1.);
        set_pixel(emboss, 1, 1, c, 1.);
        set_pixel(emboss, 1, 2, c, 1.);

        set_pixel(emboss, 2, 0, c, 0.);
        set_pixel(emboss, 2, 1, c, 1.);
        set_pixel(emboss, 2, 2, c, 2.);
    }
    return emboss;
}

float gauss_formula(int x, int y, int center, float sigma)
{
    /*!
    * The probability density function for a 2d gaussian
    */
    int sub_x = x - center;
    int sub_y = y - center;
    return 1. / (TWOPI * sigma * sigma) * exp(-1 * (sub_x * sub_x + sub_y * sub_y) / (2 * sigma * sigma));
}

image make_gaussian_filter(float sigma)
{
    /*!
    * this func takes a standard deviation value and return 
    * a filter that smooths using a gaussian with that sigma
    */

    // 99% of the probability mass for a gaussian is within +/- 3 standard deviations
    // so make the kernel be 6 times the size of sigma
    // But also we want an odd number
    // so make it be the next highest odd integer from 6x sigma.
    int size = ceil(sigma * 6);
    size = size % 2 == 0 ? size + 1 : size;
    int center = (int)size / 2;
    assert(size == center * 2. + 1.);

    image kernel = make_image(size, size, 1);

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int c = 0; c < 1; c++)
            {
                set_pixel(kernel, x, y, c, gauss_formula(x, y, center, sigma));
            }
        }
    }

    // this is a blurring filter so we want all the weights to sum to 1
    l1_normalize(kernel);

    return kernel;
}

image add_image(image a, image b)
{
    // check that the two images have the same size
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    // make the new hybrid image
    image im = make_image(a.w, a.h, a.c);

    // sum
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        im.data[i] = a.data[i] + b.data[i];
    }
    return im;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im = make_image(a.w, a.h, a.c);
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        im.data[i] = a.data[i] - b.data[i];
    }
    return im;
}

image make_gx_filter()
{
    // TODO
    return make_image(1, 1, 1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1, 1, 1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1, 1, 1);
}
