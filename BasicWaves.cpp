#include <stdlib.h>
#include <cmath>
#include "oscillator.h"
#include "global.h"

/* 
CONSTRUCTORS & SETUP FUNCTION
*/

BasicWaves::BasicWaves(float* sr)
{
	setup(sr);
}

void BasicWaves::setup(float* sr)
{
	SR = sr;
	iSR = 1.0 / *sr;
}

/*
MAIN FUNCTION
*/

float BasicWaves::process(float f)
{
	float delta = ditherFreq(&f);
	delta *= tune;
	delta *= iSR;
	
	// Calculate sample
	float p = pwPhase();
	float s = shapePhase(&p, &delta);
	
	distortPhase(&p, &s);
	
	// Increment Phase
	phase += delta;
	if ((int)phase) phase--;
	return p * lvl;
}

/* 
SETTER AND GETTER FUNCTIONS 
*/

void BasicWaves::setTune(float t)
{
	tune = t * 0.5 + 0.5;
	tune = fmaxf(tune, 0);
}

void BasicWaves::setShape(float s)
{
	s *= 3;
	shape = s;
}

void BasicWaves::setMorph(float m)
{
	dist = m;
}

void BasicWaves::setPW(float p)
{
	pw = p * 0.95 + 0.025;
	pw = p;
}

void BasicWaves::setLvl(float l)
{
	lvl = l;
}

void BasicWaves::setPhase(float p)
{
	fmath::clip(&p, 0.0, 1.0);
	phase = p;
}

float BasicWaves::getPhase() const
{
	return phase;
}

/* 
SUB ROUTINES
*/

// Pulse width control based on varying playback speeds
// between the first and second halfs of the wave period.
float BasicWaves::pwPhase() const
{
	return phase < pw ? phase * (0.5 / pw) : (phase - pw) / (2 - pw * 2) + 0.5;
}

// Phase distortion between arbitrary waveforms
// Stolen from: https://forum.pdpatchrepo.info/topic/6759/new-anti-aliasing-and-phase-distortion-abstractions
float BasicWaves::shapePhase(float* p, float* delta) const
{
	static float last;
	float waves[4];
	
	// Sawtooth is required to generate a square wave
	// Synthesize by taking phase and stretching to -1 and 1
	waves[SAW] = fmath::fast_fwrap(*p + 0.5) * 2.0 - 1.0;
	waves[SAW] -= polyBLEP(0.5 + (0.5 - pw), delta);
	
	// Square wave is required to generate all other shapes
	// Synthesize by integrating a second niave saw into the first saw
	waves[PULSE] = waves[SAW] - (*p * 2.0 - 1.0);
	waves[PULSE] += polyBLEP(0, delta);
	
	// Check if triangle or exponential are selected
	// before spending resources to synthesize them
	switch((wave)shape)
	{
		default:
			break;
		case TRI - 1:
		case TRI:
		{
			// Leak integration (Low-pass) of a square wave
			// * 32 is arbitrarily for better PWM range
			float leak = 32.0 * fmath::fast_fabs(0.5 - pw) * fmath::fast_fabs(0.5 - pw) + 1;
			leak *= *delta * M_PI * 2.0;
			waves[TRI] = fminf(leak, 0.9) * waves[PULSE];
			waves[TRI] += fmaxf(1.0 - leak, 0.1) * last;
		    last = waves[TRI];
		    break;
		}
		case EXPO - 1:
		case EXPO:
		{
			// Exponential phase value
			// amplitude modulated by the square wave
			waves[EXPO] = waves[PULSE] * fmath::fast_fwrap(*p * 2.0) * fmath::fast_fwrap(*p * 2.0);
			break;
		}
	}

	// Linear interpolation between adjacent waves, y = (1-x)a + xb
	float interpolate = fmath::fast_fwrap(shape);
	return waves[(int)shape] * (1.0 - interpolate) + waves[(int)shape + 1] * interpolate;
}

void BasicWaves::distortPhase(float* p, float* s) const
{
	// Convert shapePhase to asin of shapePhase
	fmath::fast_asin(s);
	
	// Wrap phase as a triangle wave between -0.5 and 0.5
	*p = *p + 0.5;
	fmath::fast_fwrap(p);
	*p -= 0.5;
	fmath::fast_fabs(p);
	
	// Normalize asin(shapePhase) to 1rad/period.
	// Integrate into wrapped phase and shift values to
	// return the cosine from fmath::fast_sin.
	*p = (*p + ((-0.159155 * *s - *p + 0.25) * dist)) * 2.0 - 0.5;
	fmath::fast_sin(p);
}

// polyBLEP antialiasing algorithm
// Source from here: http://www.martin-finke.de/blog/articles/audio-plugins-018-polyBLEP-BasicWaves/
float BasicWaves::polyBLEP(float p, float* delta) const
{
	p += phase;
	fmath::fast_fwrap(&p);
	
	int a = p < *delta ? -1 : (p > 1.0 - *delta ? 1 : 0); 
	
	switch(a) {
		case 0:
			return 0.0;
		case -1:
			p /= *delta;
			// 2 * (t - t^2/2 - 0.5)
			return p+p - p*p - 1.0;
		case 1:
			p = (p - 1.0) / *delta;
			// 2 * (t^2/2 + t + 0.5)
			return p+p + p*p + 1.0;
		default:
			return 0.0;
	}
}

// Frequency dithering algorithm roughly adapted to C++
// Source from here: https://forum.pdpatchrepo.info/topic/6759/new-anti-aliasing-and-phase-distortion-abstractions
float BasicWaves::ditherFreq(float* f) const
{
	static unsigned int seed = 0;
	if(*f != 0.0) {
		// Find the two closest integer ratios for samplerate/frequency
		int ratio = (int)(*SR / *f);
		float f_d0 = *SR / (ratio + 1);
		float f_d1 = *SR / ratio;
		// Dither between the two using a weighted random roll
		if ((*f - f_d0) / (f_d1 - f_d0) < fmath::fast_rand(&(seed))) {
			return f_d0;
		} else {
			return f_d1;
		}
	} else {
		// If the given frequency is 0, return 0;
		return 0.0;
	}
}