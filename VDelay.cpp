#include <cmath>
#include "delay.h"

void VDelay::process(float* in, float l, float fb)
{
	float back = (float)ptr - fmaxf(8.0, (float)max * l);
	back = back < 0.0 ? back + max : back;
	const int intBack = (int)back;
	
	int readPtr[4] {
		intBack - 1,
		intBack,
		intBack + 1,
		intBack + 2
	};
	
	readPtr[0] = readPtr[0] < 0 ? max - 1 : readPtr[0];
	readPtr[2] = readPtr[2] >= max ? 0 : readPtr[2];
	readPtr[3] = readPtr[3] >= max ? 0 : readPtr[3];
	
	const float read[4] {
		buffer[readPtr[0]],
		buffer[readPtr[1]],
		buffer[readPtr[2]],
		buffer[readPtr[3]]
	};
	
	const float x = back - (float)intBack;
	const float coef[4] {
		read[1],
		0.5f * (read[2] - read[0]),
		read[0] - 2.5f * read[1] + 2.0f * read[2] - 0.5f * read[3],
		0.5f * (read[3] - read[0]) + 1.5f * (read[1] - read[2])
	};
	
	const float out = ((coef[3] * x + coef[2]) * x + coef[1]) * x + coef[0];
	buffer[ptr] = *in + out * fb;
	
	ptr++;
	ptr = ptr >= max ? 0 : ptr;
	
	*in = out;
}

float VDelay::process(float s, float l, float fb)
{
	process(&s, l, fb);
	return s;
}