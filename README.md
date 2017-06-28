# echart2
chart library based on the EFL

# compilation

It needs meson and ninja.

## first time, no cross compilation:

mkdir build && cd build && meson ..
ninja

## first time, cross compilation (i686 with mingw-w64 in this example), in build:

mkdir build && cd build && meson .. --cross-file ../cross_i686_w64_mingw32.txt

## after the first time, in build/:

ninja

## to clean:

ninja -t clean
