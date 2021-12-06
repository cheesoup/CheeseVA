#pragma once

class KarlsenFilter {
	public:
		// Initialization
		KarlsenFilter() {}
		KarlsenFilter(float* sr);
		
		// Parameter setting
		void setup(float* sr);
		void setCutoff(float c);
		void setResonance(float r);
		
		// Main routines
		float process(float in);
		
		~KarlsenFilter() {}
	private:
		float cutoff;
		float resonance;
		float iSR;
		float buffer[5];
};