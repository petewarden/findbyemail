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

#include "vect.h"
#include "peteutil.h"

template <class T> struct Bitmap1
{
	T* _pixelData;
	int _width;
    int _height;
    int _rowBytes;
    
    Bitmap1(int width=1, int height=1, int rowBytes=0);
    Bitmap1(const Bitmap1<T>& other);
    Bitmap1(T* data, int width, int height, int rowBytes=0);
    ~Bitmap1();
    
    size_t bytesPerRow();
    size_t dataByteSize();
    
    T& pixelAt(int x, int y);

    void replaceWithData(T* data, int width, int height, int rowBytes=0);    

protected:
    void setFromData(T* data, int width, int height, int rowBytes=0);
    void freeData();
};

template <class T> struct Bitmap2
{
	T* _pixelData;
	int _width;
    int _height;
    int _rowBytes;
    
    Bitmap2(int width=1, int height=1, int rowBytes=0);
    Bitmap2(const Bitmap2<T>& other);
    Bitmap2(T* data, int width, int height, int rowBytes=0);
    ~Bitmap2();

    size_t bytesPerRow();
    size_t dataByteSize();

    Vect2<T>& pixelAt(int x, int y);
    
    void replaceWithData(T* data, int width, int height, int rowBytes=0);    

protected:
    void setFromData(T* data, int width, int height, int rowBytes=0);
    void freeData();
};

template <class T> struct Bitmap3
{
	T* _pixelData;
	int _width;
    int _height;
    int _rowBytes;
    
    Bitmap3(int width=1, int height=1, int rowBytes=0);
    Bitmap3(const Bitmap3<T>& other);
    Bitmap3(T* data, int width, int height, int rowBytes=0);
    ~Bitmap3();

    size_t bytesPerRow();
    size_t dataByteSize();

    Vect3<T>& pixelAt(int x, int y);

    void replaceWithData(T* data, int width, int height, int rowBytes=0);    

protected:
    void setFromData(T* data, int width, int height, int rowBytes=0);
    void freeData();
};

template <class T> struct Bitmap4
{
	T* _pixelData;
	int _width;
    int _height;
    int _rowBytes;
    
    Bitmap4(int width=1, int height=1, int rowBytes=0);
    Bitmap4(const Bitmap4<T>& other);
    Bitmap4(T* data, int width, int height, int rowBytes=0);
    ~Bitmap4();

    size_t bytesPerRow();
    size_t dataByteSize();

    Vect4<T>& pixelAt(int x, int y);

    void replaceWithData(T* data, int width, int height, int rowBytes=0);    

protected:
    void setFromData(T* data, int width, int height, int rowBytes=0);
    void freeData();
};

typedef struct Bitmap1<unsigned char> Bitmap1b;
typedef struct Bitmap1<float> Bitmap1f;

typedef struct Bitmap2<unsigned char> Bitmap2b;
typedef struct Bitmap2<float> Bitmap2f;

typedef struct Bitmap3<unsigned char> Bitmap3b;
typedef struct Bitmap3<float> Bitmap3f;

typedef struct Bitmap4<unsigned char> Bitmap4b;
typedef struct Bitmap4<float> Bitmap4f;

// Bitmap1 Implementation

template <class T> Bitmap1<T>::Bitmap1(int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*1);
    else
        _rowBytes = rowBytes;

    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memset(_pixelData, 0, dataSize);
}

template <class T> Bitmap1<T>::Bitmap1(const Bitmap1<T>& other)
{
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
}

template <class T> Bitmap1<T>::Bitmap1(T* data, int width, int height, int rowBytes)
{
    setFromData(data, width, height, rowBytes);
}

template <class T> void Bitmap1<T>::replaceWithData(T* data, int width, int height, int rowBytes)
{
    freeData();
    setFromData(data, width, height, rowBytes);
}    

template <class T> void Bitmap1<T>::setFromData(T* data, int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*1);
    else
        _rowBytes = rowBytes;
    
    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memcpy(_pixelData, data, dataSize);
}

template <class T> Bitmap1<T>::~Bitmap1()
{
    freeData();
}

template <class T> void Bitmap1<T>::freeData()
{
    if (_pixelData!=NULL)
        free(_pixelData);
    _pixelData = NULL;
}

template <class T> size_t Bitmap1<T>::dataByteSize()
{
    return (bytesPerRow()*_height);
}

template <class T> size_t Bitmap1<T>::bytesPerRow()
{
    return _rowBytes;
}

template <class T> T& Bitmap1<T>::pixelAt(int x, int y)
{
    // Pete - this behavior effectively repeats the border pixels infinitely for the area
    // outside the image. This is what we need for blurring algorithms, since the common
    // alternative of black for non-image pixels would darken the edges.
    x = gate(x, 0, _width);
    y = gate(y, 0, _height);
    
    T* currentPixel = (T*)((char*)(_pixelData)+(y*bytesPerRow())+(x*sizeof(T)*1));
    
    return (T)(*currentPixel);
}

// Bitmap2 Implementation

template <class T> Bitmap2<T>::Bitmap2(int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*2);
    else
        _rowBytes = rowBytes;

    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memset(_pixelData, 0, dataSize);
}

template <class T> Bitmap2<T>::Bitmap2(const Bitmap2<T>& other)
{
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
}

template <class T> Bitmap2<T>::Bitmap2(T* data, int width, int height, int rowBytes)
{
    setFromData(data, width, height, rowBytes);
}

template <class T> void Bitmap2<T>::replaceWithData(T* data, int width, int height, int rowBytes)
{
    freeData();
    setFromData(data, width, height, rowBytes);
}    

template <class T> void Bitmap2<T>::setFromData(T* data, int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*2);
    else
        _rowBytes = rowBytes;
    
    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memcpy(_pixelData, data, dataSize);
}

template <class T> Bitmap2<T>::~Bitmap2()
{
    freeData();
}

template <class T> void Bitmap2<T>::freeData()
{
    if (_pixelData!=NULL)
        free(_pixelData);
    _pixelData = NULL;
}

template <class T> size_t Bitmap2<T>::dataByteSize()
{
    return (bytesPerRow()*_height);
}

template <class T> size_t Bitmap2<T>::bytesPerRow()
{
    return _rowBytes;
}

template <class T> inline Vect2<T>& Bitmap2<T>::pixelAt(int x, int y)
{
    // Pete - this behavior effectively repeats the border pixels infinitely for the area
    // outside the image. This is what we need for blurring algorithms, since the common
    // alternative of black for non-image pixels would darken the edges.
    x = gate(x, 0, _width);
    y = gate(y, 0, _height);
    
    T* currentPixel = (T*)((char*)(_pixelData)+(y*bytesPerRow())+(x*sizeof(T)*2));
    
    return (Vect2<T>)(*currentPixel);
}

// Bitmap3 Implementation

template <class T> Bitmap3<T>::Bitmap3(int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*3);
    else
        _rowBytes = rowBytes;

    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memset(_pixelData, 0, dataSize);
}

template <class T> Bitmap3<T>::Bitmap3(const Bitmap3<T>& other)
{
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
}

template <class T> Bitmap3<T>::Bitmap3(T* data, int width, int height, int rowBytes)
{
    setFromData(data, width, height, rowBytes);
}

template <class T> void Bitmap3<T>::replaceWithData(T* data, int width, int height, int rowBytes)
{
    freeData();
    setFromData(data, width, height, rowBytes);
}    

template <class T> void Bitmap3<T>::setFromData(T* data, int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*3);
    else
        _rowBytes = rowBytes;
    
    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memcpy(_pixelData, data, dataSize);
}

template <class T> Bitmap3<T>::~Bitmap3()
{
    freeData();
}

template <class T> void Bitmap3<T>::freeData()
{
    if (_pixelData!=NULL)
        free(_pixelData);
    _pixelData = NULL;
}

template <class T> size_t Bitmap3<T>::dataByteSize()
{
    return (bytesPerRow()*_height);
}

template <class T> size_t Bitmap3<T>::bytesPerRow()
{
    return _rowBytes;
}

template <class T> inline Vect3<T>& Bitmap3<T>::pixelAt(int x, int y)
{
    // Pete - this behavior effectively repeats the border pixels infinitely for the area
    // outside the image. This is what we need for blurring algorithms, since the common
    // alternative of black for non-image pixels would darken the edges.
    x = gate(x, 0, _width);
    y = gate(y, 0, _height);
    
    T* currentPixel = (T*)((char*)(_pixelData)+(y*bytesPerRow())+(x*sizeof(T)*3));
    
    return (Vect3<T>)(*currentPixel);
}

// Bitmap4 Implementation

template <class T> Bitmap4<T>::Bitmap4(int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*4);
    else
        _rowBytes = rowBytes;

    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memset(_pixelData, 0, dataSize);
}

template <class T> Bitmap4<T>::Bitmap4(const Bitmap4<T>& other)
{
    setFromData(other._pixelData, other._width, other._height, other._rowBytes);
}

template <class T> Bitmap4<T>::Bitmap4(T* data, int width, int height, int rowBytes)
{
    setFromData(data, width, height, rowBytes);
}

template <class T> void Bitmap4<T>::replaceWithData(T* data, int width, int height, int rowBytes)
{
    freeData();
    setFromData(data, width, height, rowBytes);
}    

template <class T> void Bitmap4<T>::setFromData(T* data, int width, int height, int rowBytes)
{
    _width = width;
    _height = height;
    if (rowBytes==0)
        _rowBytes = (_width*sizeof(T)*4);
    else
        _rowBytes = rowBytes;
    
    const size_t dataSize = dataByteSize(); 
    _pixelData = (T*)(malloc(dataSize));
    memcpy(_pixelData, data, dataSize);
}

template <class T> Bitmap4<T>::~Bitmap4()
{
    freeData();
}

template <class T> void Bitmap4<T>::freeData()
{
    if (_pixelData!=NULL)
        free(_pixelData);
    _pixelData = NULL;
}

template <class T> size_t Bitmap4<T>::dataByteSize()
{
    return (bytesPerRow()*_height);
}

template <class T> size_t Bitmap4<T>::bytesPerRow()
{
    return _rowBytes;
}

template <class T> inline Vect4<T>& Bitmap4<T>::pixelAt(int x, int y)
{
    // Pete - this behavior effectively repeats the border pixels infinitely for the area
    // outside the image. This is what we need for blurring algorithms, since the common
    // alternative of black for non-image pixels would darken the edges.
    x = gate(x, 0, _width);
    y = gate(y, 0, _height);
    
    T* currentPixel = (T*)((char*)(_pixelData)+(y*bytesPerRow())+(x*sizeof(T)*4));
    
    return (Vect4<T>)(currentPixel);
}

#endif // INCLUDE_BITMAP_H