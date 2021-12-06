#pragma once

class BasicWaves {
	public:
	    enum wave {
	    	NOTHING = -1,
	        TRI = 0,
	        PULSE,
	        EXPO,
	        SAW
	    };
	    
		// Initialization
		BasicWaves() {}
		BasicWaves(float* sr);
		
		// Parameter setting
		void setup(float* sr);
		void setMorph(float m);
		void setShape(float s);
		void setPW(float pw);
		void setLvl(float l);
		void setPhase(float p);
		void setTune(float t);
		
		float getPhase() const;
		
		// Main routines
		float process(float f);
		
		~BasicWaves() {}
		
	private:
		float tune = 1.0;
		float pw = 0.5;
		float lvl = 1;
		float dist = 0;
		float shape = 0;
		
		float phase = 0;
		const float* SR;
		float iSR;
		
		// Wave Shapers
		float pwPhase() const;
		float shapePhase(float* p, float* delta) const;
		void distortPhase(float* p, float* s) const;
		
		// Anti-Aliasing
		float ditherFreq(float* f) const;
		float polyBLEP(float ht, float* delta) const;
};