#ifndef INCLUDE_BITMAP_H
#define INCLUDE_BITMAP_H
/*
 *  bitmap.h
 *  isightviewer
 *
 *  Created by Pete Warden on 1/5/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "vect.h"
#include "peteutil.h"

template <class T, int channels> struct Bitmap
{
	T* _pixelData;
	int _width;
    int _height;
    int _rowBytes;
    
    Bitmap(int width=1, int height=1, int rowBytes=0);
    Bitmap(const Bitmap<T, channels>& other);
    Bitmap(T* data, int width, int height, int rowBytes=0);
    Bitmap<T, channels>& operator=(const Bitmap<T, channels>& other);
    ~Bitmap();
    
    size_t bytesPerRow();
    size_t dataByteSize();
    
    T* pixelAt(int x, int y);

    void replaceWithData(T* data, int width, int height, int rowBytes=0);    
    void resizeData(int width, int height, int rowBytes=0);
    void zeroFillData();

protected:
    void allocateData(int width, int height, int rowBytes=0);
    void setFromData(T* data, int width, int height, int rowBytes=0);
    void freeData();
};

typedef struct Bitmap<unsigned char, 1> Bitmap1b;
typedef struct Bitmap<float, 1> Bitmap1f;

typedef struct Bitmap<unsigned char, 2> Bitmap2b;
typedef struct Bitmap<float, 2> Bitmap2f;

typedef struct Bitmap<unsigned char, 3> Bitmap3b;
typedef struct Bitmap<float, 3> Bitmap3f;

typedef struct Bitmap<unsigned char, 4> Bitmap4b;
typedef struct Bitmap<float, 4> Bitmap4f;

// Implementation

template <class T, int channels> Bitmap<T, channels>::Bitmap(int width, int height, int rowBytes)
{
    allocateData(width, height, rowBytes);
    zeroFillData();
}

template <class T, int channels> Bitmap<T, channels>::Bitmap(const Bitmap<T, channels>& other)
{
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
}

template <class T, int channels> Bitmap<T, channels>& Bitmap<T, channels>::operator=(const Bitmap<T, channels>& other)
{
    freeData();
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
    return *this;
}

template <class T, int channels> Bitmap<T, channels>::Bitmap(T* data, int width, int height, int rowBytes)
{
    setFromData(data, width, height, rowBytes);
}

template <class T, int channels> void Bitmap<T, channels>::replaceWithData(T* data, int width, int height, int rowBytes)
{
    freeData();
    setFromData(data, width, height, rowBytes);
}    

template <class T, int channels> void Bitmap<T, channels>::setFromData(T* data, int width, int height, int rowBytes)
{
    allocateData(width, height, rowBytes);
    memcpy(_pixelData, data, dataByteSize());
}

template <class T, int channels> void Bitmap<T, channels>::resizeData(int width, int height, int rowBytes)
{
    freeData();
    allocateData(width, height, rowBytes);
}

template <class T, int channels> void Bitmap<T, channels>::allocateData(int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*channels);
    else
        _rowBytes = rowBytes;

    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
}

template <class T, int channels> void Bitmap<T, channels>::zeroFillData()
{
    memset(_pixelData, 0, dataByteSize());
}

template <class T, int channels> Bitmap<T, channels>::~Bitmap()
{
    freeData();
}

template <class T, int channels> void Bitmap<T, channels>::freeData()
{
    if (_pixelData!=NULL)
        free(_pixelData);
    _pixelData = NULL;
}

template <class T, int channels> size_t Bitmap<T, channels>::dataByteSize()
{
    return (bytesPerRow()*_height);
}

template <class T, int channels> size_t Bitmap<T, channels>::bytesPerRow()
{
    return _rowBytes;
}

template <class T, int channels> T* Bitmap<T, channels>::pixelAt(int x, int y)
{
    // Pete - this behavior effectively repeats the border pixels infinitely for the area
    // outside the image. This is what we need for blurring algorithms, since the common
    // alternative of black for non-image pixels would darken the edges.
    x = gate(x, 0, (_width-1));
    y = gate(y, 0, (_height-1));
    
    T* currentPixel = (T*)((char*)(_pixelData)+(y*bytesPerRow())+(x*sizeof(T)*channels));
    
    return currentPixel;
}

#endif // INCLUDE_BITMAP_H