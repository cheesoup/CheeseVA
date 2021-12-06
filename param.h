#pragma once

class Param {
	public:
		Param() {}
		Param(float *sr, float *s);
		void setup(float *sr, float *s);
		
		void process();
		
		void setTarget(float v);
		void setCurrent(float v);
		
		float getCurrent() const;
		float getTarget() const;
		bool getStatus() const;
	
	private:
		float target = 0;
		float current = 0;
		float delta = 0;
		
		unsigned int count;
		unsigned int steps;
};