#!/bin/sh

gcc -arch i386 -framework GLUT -framework OpenGL -framework QuickTime -framework ApplicationServices -lm -lstdc++ isightviewer.cpp livefeed_osx.c -o isightviewer