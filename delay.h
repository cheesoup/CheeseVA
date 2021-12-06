#pragma once

class Delay {
	public:
		Delay() {}
		Delay(float* sr, float l);
		virtual void setup(float* sr, float l);
		
		virtual void process(float* in);
		virtual float process(float in);
		
		int getMax();
		int getPointer();
		
		virtual ~Delay() {
			delete[] buffer;
		};
		
	protected:
		float* buffer;
		float* SR;
		unsigned int ptr = 0;
		unsigned int max;
};

class VDelay: public Delay {
	using Delay::Delay;
	using Delay::process;
	
	public:
		void process(float* in, float l, float fb);
		float process(float in, float l, float fb);
};