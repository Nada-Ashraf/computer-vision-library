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
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    // TODO
    return make_image(1, 1, 1);
}

image add_image(image a, image b)
{
    // TODO
    return make_image(1, 1, 1);
}

image sub_image(image a, image b)
{
    // TODO
    return make_image(1, 1, 1);
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
