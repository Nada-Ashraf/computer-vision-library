from uwimg import *

# 1. Resizing using nearest neighbour interpolation
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*5, im.h*5)
save_image(a, "./results/dog4x-nn")

# 2. Resizing using bilinear interpolation
im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "./results/dog4x-bl")

# 3. Convolution blur
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "./results/dog-box7")

# 4. Blur and Resize
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "./results/dogthumb")

# 5. Highpass
im = load_image("data/dog.jpg")
f = make_highpass_filter()
highpass = convolve_image(im, f, 0)
save_image(highpass, "./results/dog_highpass")

# 6. Sharpen
im = load_image("data/dog.jpg")
f = make_sharpen_filter()
sharpen = convolve_image(im, f, 1)
save_image(sharpen, "./results/dog_sharpen")

# 7. Emboss
im = load_image("data/dog.jpg")
f = make_emboss_filter()
emboss = convolve_image(im, f, 1)
save_image(emboss, "./results/dog_emboss")

# 8. Gauss smoothing
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "./results/dog-gauss2")

# 9. Hybrid images
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "./results/low-frequency")
save_image(hfreq, "./results/high-frequency")
save_image(reconstruct, "./results/reconstruct")

# 10. Calculate gradient magnitude
im = load_image("data/dog.jpg")
res = sobel_image(im)
mag = res[0]
feature_normalize(mag)
save_image(mag, "./results/magnitude")

# 11. Style image
im = load_image("data/dog.jpg")
styled = colorize_sobel(im)
save_image(styled, "./results/styled")
