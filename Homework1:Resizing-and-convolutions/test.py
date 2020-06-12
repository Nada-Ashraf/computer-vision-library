from uwimg import *

im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*5, im.h*5)
save_image(a, "./results/dog4x-nn")
