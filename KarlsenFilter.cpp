#include <cmath>
#include "global.h"
#include "filter.h"

KarlsenFilter::KarlsenFilter(float* sr)
{
	setup(sr);
}

void KarlsenFilter::setup(float* sr)
{
	iSR = 1.0 / *sr;
	cutoff = 0.8;
	resonance = 0.0;
	for (int i = 0; i < sizeof(&buffer); i++)
	{
		buffer[i] = 0;
	}
}

void KarlsenFilter::setCutoff(float c)
{
	cutoff = (c * M_PI) * iSR;
	fmath::clip(&cutoff, 0.8, 0.0);
}

void KarlsenFilter::setResonance(float r)
{
	resonance = r;
}

float KarlsenFilter::process(float in)
{
	buffer[4] = ((buffer[1] - buffer[4]) * cutoff) + buffer[4];
	float fb = (buffer[3] - buffer[4]) * resonance * M_E * 2;
	fmath::clip(&fb, 1.0, -1.0);
	in -= fb;
	buffer[0] = ((in - buffer[0]) * cutoff) + buffer[0];
	buffer[1] = ((buffer[0] - buffer[1]) * cutoff) + buffer[1];
	buffer[2] = ((buffer[1] - buffer[2]) * cutoff) + buffer[2];
	buffer[3] = ((buffer[2] - buffer[3]) * cutoff) + buffer[3];
	
	float out = buffer[3];
	fmath::sigmoid(&out);
	return out;
}