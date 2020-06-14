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
    im.data[x + im.w * y + im.w * im.h * c] = v;
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
    /*!
    * to avoid overflow
    * make sure the pixel values in the image stay between 0 and 1.
    */
    for (int c = 0; c < im.c; c++)
    {
        for (int h = 0; h < im.h; h++)
        {
            for (int w = 0; w < im.w; w++)
            {
                // get pixel value
                float v = get_pixel(im, w, h, c);

                // any value below zero gets set to zero
                // and any value above 1 gets set to one.
                v = v > 1.0 ? 1.0 : v;
                v = v < 0.0 ? 0.0 : v;
                set_pixel(im, w, h, c, v);
            }
        }
    }
}

float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

float compute_value(float R, float G, float B)
{
    return three_way_max(R, G, B);
}
float compute_saturation(float V, float R, float G, float B)
{
    float m = three_way_min(R, G, B);
    float C = V - m;
    if (V > 0)
    {
        return C / V;
    }
    else
    {
        return 0;
    }
}

float compute_hue(float V, float R, float G, float B)
{
    float H;
    float m = three_way_min(R, G, B);
    float C = V - m;
    float H_hatch = 0;
    if (C == 0)
    {
        H = 0;
    }
    else
    {
        if (V == R)
            H_hatch = (G - B) / C;
        if (V == G)
            H_hatch = (B - R) / C + 2;
        if (V == B)
            H_hatch = (R - G) / C + 4;
        H = H_hatch / 6.;
        if (H_hatch < 0)
            H = H + 1;
    }
    return H;
}

void rgb_to_hsv(image im)
{
    for (int y = 0; y < im.h; y++)
    {
        for (int x = 0; x < im.w; x++)
        {
            // get pixel values
            float R = get_pixel(im, x, y, 0);
            float G = get_pixel(im, x, y, 1);
            float B = get_pixel(im, x, y, 2);

            // compute Value
            float V = compute_value(R, G, B);

            // compute Saturation
            float S = compute_saturation(V, R, G, B);

            // compute Hue
            float H = compute_hue(V, R, G, B);

            // set pixels
            set_pixel(im, x, y, 0, H);
            set_pixel(im, x, y, 1, S);
            set_pixel(im, x, y, 2, V);
        }
    }
}

void hsv_to_rgb(image im)
{
    float R, G, B;
    for (int h = 0; h < im.h; h++)
    {
        for (int w = 0; w < im.w; w++)
        {
            float H = get_pixel(im, w, h, 0);
            float S = get_pixel(im, w, h, 1);
            float V = get_pixel(im, w, h, 2);

            float C = S * V;
            float m = V - C;

            float H_ = H * 6.;
            float X = C * (1 - fabs(fmod(H_, 2) - 1));

            if (0. <= H_ && H_ <= 1.)
            {
                R = C;
                G = X;
                B = 0;
            }
            else if (1. <= H_ && H_ <= 2.)
            {
                R = X;
                G = C;
                B = 0;
            }
            else if (2. <= H_ && H_ <= 3.)
            {
                R = 0;
                G = C;
                B = X;
            }
            else if (3. <= H_ && H_ <= 4.)
            {
                R = 0;
                G = X;
                B = C;
            }
            else if (4. <= H_ && H_ <= 5.)
            {
                R = X;
                G = 0;
                B = C;
            }
            else if (5. <= H_ && H_ <= 6.)
            {
                R = C;
                G = 0;
                B = X;
            }
            else
            {
                R = 0;
                G = 0;
                B = 0;
            }
            R = R + m;
            G = G + m;
            B = B + m;
            set_pixel(im, w, h, 0, R);
            set_pixel(im, w, h, 1, G);
            set_pixel(im, w, h, 2, B);
        }
    }
}
