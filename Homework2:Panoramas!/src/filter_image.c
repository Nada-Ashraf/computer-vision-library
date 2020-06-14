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
    image highpass = make_box_filter(3);
    highpass.data[0] = 0;
    highpass.data[1] = -1;
    highpass.data[2] = 0;
    highpass.data[3] = -1;
    highpass.data[4] = 4;
    highpass.data[5] = -1;
    highpass.data[6] = 0;
    highpass.data[7] = -1;
    highpass.data[8] = 0;

    return highpass;
}

image make_sharpen_filter()
{
    image sharpen = make_box_filter(3);
    sharpen.data[0] = 0;
    sharpen.data[1] = -1;
    sharpen.data[2] = 0;
    sharpen.data[3] = -1;
    sharpen.data[4] = 5;
    sharpen.data[5] = -1;
    sharpen.data[6] = 0;
    sharpen.data[7] = -1;
    sharpen.data[8] = 0;

    return sharpen;
}

image make_emboss_filter()
{
    image emboss = make_box_filter(3);
    emboss.data[0] = -2;
    emboss.data[1] = -1;
    emboss.data[2] = 0;
    emboss.data[3] = -1;
    emboss.data[4] = 1;
    emboss.data[5] = 1;
    emboss.data[6] = 0;
    emboss.data[7] = 1;
    emboss.data[8] = 2;

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
    image sobel_filter = make_box_filter(3);
    sobel_filter.data[0] = -1;
    sobel_filter.data[1] = 0;
    sobel_filter.data[2] = 1;
    sobel_filter.data[3] = -2;
    sobel_filter.data[4] = 0;
    sobel_filter.data[5] = 2;
    sobel_filter.data[6] = -1;
    sobel_filter.data[7] = 0;
    sobel_filter.data[8] = 1;

    return sobel_filter;
}

image make_gy_filter()
{
    image sobel_filter = make_box_filter(3);
    sobel_filter.data[0] = -1;
    sobel_filter.data[1] = -2;
    sobel_filter.data[2] = -1;
    sobel_filter.data[3] = 0;
    sobel_filter.data[4] = 0;
    sobel_filter.data[5] = 0;
    sobel_filter.data[6] = 1;
    sobel_filter.data[7] = 2;
    sobel_filter.data[8] = 1;

    return sobel_filter;
}

void feature_normalize(image im)
{
    // for comparison
    float maxi = -INFINITY;
    float mini = +INFINITY;

    // find max and min
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        maxi = fmax(im.data[i], maxi);
        mini = fmin(im.data[i], mini);
    }

    float range = maxi - mini;
    float normalizing_pixel;
    for (int w = 0; w < im.w; w++)
    {
        for (int h = 0; h < im.h; h++)
        {
            for (int c = 0; c < im.c; c++)
            {
                float pixel = get_pixel(im, w, h, c);
                if (range != 0.)
                {
                    normalizing_pixel = (pixel - mini) / range;
                }
                else
                {
                    // avoid zero division
                    normalizing_pixel = 0;
                }
                set_pixel(im, w, h, c, normalizing_pixel);
            }
        }
    }
}

image *sobel_image(image im)
{
    /*!
    * return two images, the gradient magnitude and direction
    */

    // allocate and zero-initialize array
    image *mag_and_direct = calloc(2, sizeof(image));
    mag_and_direct[0] = make_image(im.w, im.h, 1);
    mag_and_direct[1] = make_image(im.w, im.h, 1);

    // make the filters
    image sobel_gx = convolve_image(im, make_gx_filter(), 0);
    image sobel_gy = convolve_image(im, make_gy_filter(), 0);

    for (int w = 0; w < im.w; w++)
    {
        for (int h = 0; h < im.h; h++)
        {
            for (int c = 0; c < 1; c++)
            {
                float x = get_pixel(sobel_gx, w, h, c);
                float y = get_pixel(sobel_gy, w, h, c);

                // magnitude
                set_pixel(mag_and_direct[0], w, h, 0, sqrt(x * x + y * y));

                // direction
                set_pixel(mag_and_direct[1], w, h, 0, atan2(y, x));
            }
        }
    }
    return mag_and_direct;
}

image colorize_sobel(image im)
{
    image *mag_and_direct = sobel_image(im);
    image colorize_sobel = make_image(im.w, im.h, im.c);
    feature_normalize(mag_and_direct[0]);
    feature_normalize(mag_and_direct[1]);
    float S, V, H;
    for (int w = 0; w < im.w; w++)
    {
        for (int h = 0; h < im.h; h++)
        {
            // use the magnitude to specify the
            // saturation and value of an image
            // and the angle to specify the hue
            S = get_pixel(mag_and_direct[0], w, h, 0);
            V = get_pixel(mag_and_direct[0], w, h, 0);
            H = get_pixel(mag_and_direct[1], w, h, 0);
            set_pixel(colorize_sobel, w, h, 0, H);
            set_pixel(colorize_sobel, w, h, 1, S);
            set_pixel(colorize_sobel, w, h, 2, V);
        }
    }
    hsv_to_rgb(colorize_sobel);
    return colorize_sobel;
}
