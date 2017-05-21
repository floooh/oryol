#!/bin/sh
# helper bash script to generate gl files
#
python3 ../../../../../flextgl/flextGLgen.py -T glfw3 -D . flextgl_profile.txt 
