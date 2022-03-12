#! /bin/make
# :vim: set noexpandtab:
CC=gcc
CFLAGS -Wall -O2
LFLAGS = -lm `sdl2-config --cflags --libs`

mandelbrot-sdl: mandelbrot-sdl.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS)

