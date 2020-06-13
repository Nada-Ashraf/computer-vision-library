from uwimg import *

# 1. Resizing using nearest neighbour interpolation
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*5, im.h*5)
save_image(a, "./results/dog4x-nn")

# 2. Resizing using bilinear interpolation
im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "./results/dog4x-bl")
