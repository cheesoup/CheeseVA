#include "voice.h" 

class VoiceManager {
	public:
		VoiceManager() {}
		VoiceManager(float sr);
		void setup(float sr);
		float process();
		
		void on(int note, int vel);
		void off(int note);
		
		void setMorph(float m, int osc);
		void setShape(float s, int osc);
		void setPW(float pw, int osc);
		void setLevel(float l, int osc);
		void setTune(float t, int osc);
		void setMix(float m);
		void setFM(float fm);
		
		void setCutoff(float c);
		void setResonance(float r);
		
		void setEnvelope(float v, Envelope::stage s);
	
	private:
		static const int total = 4;
		Voice voice[total];
		Voice* getFree();
		
		float SR;
};
