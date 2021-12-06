#pragma once

class Envelope {
	public:	
		// Constants
	    enum stage {
	        OFF = 0,
	        ATTACK,
	        DECAY,
	        SUSTAIN,
	        RELEASE,
	        STAGETOTAL
	    };
		const float min_lvl = 0.0001;
		const float min_time = 0.01;
	    
		// Constructor
	    Envelope() {}
		Envelope(float* sr);
		
		// Setup
		void setup();
		void setup(float* sr);
		
		// Do the thing
		float process();
		void noteOn(float v);
		void noteOff();
		
		// Parameters
		void setStage(float v, stage s);
		stage getStage() const;
		
		// Deconstructor
		~Envelope(){};
	private:
		stage curStage;
		unsigned int current;
		float lvl = min_lvl;
		float vel;
		float mult;
		float* SR;
	    float adsr[STAGETOTAL];
	    
		void go(stage next);
	    void calcMult(float start, float end, unsigned int l);
};