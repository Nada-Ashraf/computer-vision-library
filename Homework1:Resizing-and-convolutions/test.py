from uwimg import *

# 1. Resizing using nearest neighbour interpolation
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*5, im.h*5)
save_image(a, "./results/dog4x-nn")

# 2. Resizing using bilinear interpolation
im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "./results/dog4x-bl")

# 3. Convolution
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "./results/dog-box7")

im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "./results/dogthumb")

# 3. Gauss smoothing
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "./results/dog-gauss2")

# 4. Hybrid images
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "./results/low-frequency")
save_image(hfreq, "./results/high-frequency")
save_image(reconstruct, "./results/reconstruct")
