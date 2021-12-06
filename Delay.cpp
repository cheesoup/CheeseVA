#include "delay.h"

Delay::Delay(float* sr, float l)
{
	setup(sr, l);
}

void Delay::setup(float* sr, float l)
{
	SR = sr;
	max = (int)(l * *sr);
	buffer = new float[max] { 0.0 };
}

void Delay::process(float* in)
{
	float out = buffer[ptr];
	buffer[ptr] = *in;
	ptr++;
	ptr -= (ptr >= max) * max;
	
	*in = out;
}

float Delay::process(float in)
{
	float out = buffer[ptr];
	buffer[ptr] = in;
	ptr++;
	ptr -= (ptr >= max) * max;
	
	return out;
}

int Delay::getMax() 
{
	return max;
}

int Delay::getPointer() 
{
	return ptr;
}