#pragma once
#include "oscillator.h"
#include "filter.h"
#include "envelope.h"

class Voice {
	public:
		friend class VoiceManager;
		
		Voice() {}
		Voice(float* sr);
		
		void setup(float* sr);
		void reset();
		
		void setNote(int n);
		
		float process();
		
		~Voice() {}
	private:
		int note = 0;
		float freq = 0.0;
		float fm = 0.0;
		float mix = 0.5;
		bool active = false;
	
		BasicWaves osc[2];
		KarlsenFilter filt;
		Envelope env;
};