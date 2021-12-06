#include <cmath>
#include "voice.h"
#include "global.h"

Voice::Voice(float* sr)
{
	setup(sr);
}

void Voice::setup(float* sr)
{
	osc[0].setup(sr);
	osc[1].setup(sr);
	filt.setup(sr);
	env.setup(sr);
}

void Voice::reset()
{
	osc[0].setPhase(0);
	osc[1].setPhase(0);
}

float Voice::process()
{	
	float v = env.process();
		
	// Get oscillator processed through filter
	float out = 0;
	if (env.getStage() != Envelope::OFF) {
		float modulator = osc[1].process(freq);
		out = osc[0].process(freq + fmath::fast_fabs(modulator) * freq * fm);
		out = out * mix + modulator * (1.0 - mix);
	}
	out = filt.process(out);
	return out * v;
}

void Voice::setNote(int n)
{
    note = n;
    freq = 440.0 * pow(2.0, (note - 69.0) / 12.0);
}