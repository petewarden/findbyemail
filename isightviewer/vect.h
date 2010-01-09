#ifndef INCLUDE_VECT_H
#define INCLUDE_VECT_H
/*
 *  vect.h
 *  isightviewer
 *
 *  Created by Pete Warden on 1/5/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

template <class T> struct Vect2
{
	T x;
	T y;

	Vect2() : x(0), y(0) { }
	Vect2(T _x, T _y) : x(_x), y(_y) { }
	Vect2(const Vect2<T>& other) : x(other.x), y(other.y) { }

	const Vect2<T>& operator*=(T factor) { x*=factor; y*=factor; return *this; }
	const Vect2<T>& operator+=(const Vect2<T>& other) { x+=other.x; y+=other.y; return *this; }
	const Vect2<T>& operator-=(const Vect2<T>& other) { x-=other.x; y-=other.y; return *this; }

	Vect2<T> operator*(T factor) { return Vect2<T>(x*factor, y*factor); }
	Vect2<T> operator+(const Vect2<T>& other) { return Vect2<T>(x+other.x, y+other.y); }
	Vect2<T> operator-(const Vect2<T>& other) { return Vect2<T>(x-other.x, y-other.y); }
	
	Vect2<T> cross() { return Vect2<T>(-this->y, this->x); }

    T lengthSquared() { return ((x*x)+(y*y)); }

	Vect2<T> normalized() 
    {
        const T lengthSquared = this->lengthSquared();
        if (lengthSquared==0)
            return *this;
      
        const T length = sqrt(lengthSquared);  
        const T recipLength = (1/length);
    
        return (*this*recipLength);
    }
		
	T dot(const Vect2<T>& other) { return (x*other.x)+(y*other.y); }
};

template <class T> inline Vect2<T> operator*(T factor, const Vect2<T>& other) { 
	return Vect2<T>(factor*other.x, factor*other.y); }

template <class T> struct Vect3
{
	T x;
	T y;
	T z;

	Vect3() : x(0), y(0), z(0) { }
	Vect3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { }
	Vect3(const Vect3<T>& other) : x(other.x), y(other.y), z(other.z) { }

	const Vect3<T>& operator*=(T factor) { x*=factor; y*=factor; z*=factor; return *this; }
	const Vect3<T>& operator+=(const Vect3<T>& other) { x+=other.x; y+=other.y; z+=other.z; return *this; }
	const Vect3<T>& operator-=(const Vect3<T>& other) { x-=other.x; y-=other.y; z-=other.z; return *this; }

	Vect3<T> operator*(T factor) { return Vect3<T>(x*factor, y*factor, z*factor); }
	Vect3<T> operator+(const Vect3<T>& other) { return Vect3<T>(x+other.x, y+other.y, z+other.z); }
	Vect3<T> operator-(const Vect3<T>& other) { return Vect3<T>(x-other.x, y-other.y, z-other.z); }
	
	Vect3<T> cross(const Vect3<T>& other) { return Vect3<T>((y*other.z)-(z*other.y), 
		(z*other.x)-(x*other.z), (x*other.y)-(y*other.x)); }
		
	T dot(const Vect3<T>& other) { return (x*other.x)+(y*other.y)+(z*other.z); }
};

template <class T> inline Vect3<T> operator*(T factor, const Vect3<T>& other) { 
	return Vect3<T>(factor*other.x, factor*other.y, factor*other.z); }

template <class T> struct Vect4
{
	T x;
	T y;
	T z;
	T w;

	Vect4() : x(0), y(0), z(0), w(0) { }
	Vect4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) { }
	Vect4(const Vect4<T>& other) : x(other.x), y(other.y), z(other.z), w(other.w) { }

	const Vect4<T>& operator*=(T factor) { x*=factor; y*=factor; z*=factor; w*=factor; return *this; }
	const Vect4<T>& operator+=(const Vect4<T>& other) { x+=other.x; y+=other.y; z+=other.z; w+=other.w; return *this; }
	const Vect4<T>& operator-=(const Vect4<T>& other) { x-=other.x; y-=other.y; z-=other.z; w-=other.w; return *this; }

	Vect4<T> operator*(T factor) { return Vect4<T>(x*factor, y*factor, z*factor, w*factor); }
	Vect4<T> operator+(const Vect4<T>& other) { return Vect4<T>(x+other.x, y+other.y, z+other.z, w+other.w); }
	Vect4<T> operator-(const Vect4<T>& other) { return Vect4<T>(x-other.x, y-other.y, z-other.z, w-other.w); }
			
	T dot(const Vect4<T>& other) { return (x*other.x)+(y*other.y)+(z*other.z)+(w*other.w); }
};

template <class T> inline Vect4<T> operator*(T factor, const Vect4<T>& other) { 
	return Vect4<T>(factor*other.x, factor*other.y, factor*other.z, factor*other.w); }

typedef struct Vect2<unsigned char> Vect2b;
typedef struct Vect2<int> Vect2i;
typedef struct Vect2<float> Vect2f;
typedef struct Vect2<double> Vect2d;

typedef struct Vect3<unsigned char> Vect3b;
typedef struct Vect3<int> Vect3i;
typedef struct Vect3<float> Vect3f;
typedef struct Vect3<double> Vect3d;

typedef struct Vect4<unsigned char> Vect4b;
typedef struct Vect4<int> Vect4i;
typedef struct Vect4<float> Vect4f;
typedef struct Vect4<double> Vect4d;
    
#endif // INCLUDE_VECT_H