#pragma once

#ifndef fmath
#define fmath
namespace fmath {
	// Fast arcsin approximation. Saves about 5% per oscillator.
	// Stolen from here:
	// https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
	// Graph: https://www.desmos.com/calculator/yoq4xjnw01
	static inline void fast_asin(float* x) {
		*x = (-0.69813170079773212 * *x * *x - 0.87266462599716477) * *x;
	};
	
	// Polynomial Sine approximation, assuming x is between -0.25 and 0.25.
	// x = ((((-0.540347x^2) + 2.53566)x^2 - 5.16651)x^2 + 3.14159)x
	// https://forum.bela.io/d/83-lookup-tables-for-reduced-cpu-load/2
	static inline void fast_sin(float* x) {
		*x = ((((-0.540347 * *x * *x) + 2.53566) * *x * *x - 5.16651) * *x * *x + M_PI) * *x;
	};
	
	// Fast floating pt wrap between 0 and 1. Faster than fmodf_neon(x, 1.0) but with absolutely no flexibility.
	// Assumes a positive value. To correct negative values, add one conditionally.
	static inline void fast_fwrap(float* x) {
		*x -= (int)*x;
	}
	
	static inline float fast_fwrap(float x) {
		x -= (int)x;
		return x;
	}
	
	// Fast floating point absolute value. Not sure if it actually saves anything.
	// https://www.musicdsp.org/en/latest/Other/132-fast-abs-neg-sign-for-32bit-floats.html
	static inline void fast_fabs(float* x) {
		int i = ((*(int*)x)&0x7fffffff);
		*x = (*(float*)&i);
	}
	
	static inline float fast_fabs(float x) {
		int i = ((*(int*)&x)&0x7fffffff);
		return (*(float*)&i);
	}
	
	// Fast floating point pow function approximation.
	// I have no idea how this works other than it's some IEEE-754 thing
	// https://gist.github.com/XProger/433701300086245e0583
	static inline float fast_fpow(float a, float b) {
		union { float d; int x; } u = { a };
		u.x = (int)(b * (u.x - 1064866805) + 1064866805);
		return u.d;
	}
	
	// Return float between 0 and 1
	// Apparently this is twice as fast as (float)rand()/RAND_MAX.
	// Stolen from here. Modified to produce floats.
	// https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
	static inline float fast_rand(unsigned int* s) { 
	  *s = (214013**s+2531011);
	  return ((float)((*s>>16)&0x7FFF)) * 0.00003051944; // 0.00003051944 = 1 / (32767 - 1)
	}
	
	// What I think is the fastest sigmoid function calculate? Could be wrong.
	// y = x / (abs(1) + 1)
	static inline void sigmoid(float* x) {
		*x = *x / (fast_fabs(*x) + 1);
	}
	
	// Simple clip function
	static inline float clip(float x, float max, float min) {
		return fminf(max, fmaxf(min, x));
	}
	
	static inline void clip(float* x, float max, float min) {
		*x = fminf(max, fmaxf(min, *x));
	}
	
	// One pole LP
	// HP = x - LP
	
	static inline float onepole(float x, float c) {
		static float buffer;
		buffer = (1 - c) * x - c * buffer;
		return buffer;
	}
	
	static inline void onepole(float* x, float c) {
		static float buffer;
		buffer = (1 - c) * *x - c * buffer;
		*x = buffer;
	}
}
#endif