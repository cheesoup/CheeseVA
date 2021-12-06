#include <Bela.h>
#include <stdio.h>
#include "voice.h"
#include "voicemanager.h"

VoiceManager::VoiceManager(float sr) {
	setup(sr);
}

void VoiceManager::setup(float sr) {
	SR = sr;
	for(int i = 0; i < total; i++) {
		voice[i].setup(&SR);
	}
}

void VoiceManager::on(int note, int vel) {
	 Voice* v = getFree();
	 if (!v) return;
	 v->active = true;
	 if (v->env.getStage() == 0) v->reset();
	 v->setNote(note);
	 v->env.noteOn(1);
}

void VoiceManager::off(int note) {
    // Find the voice(s) with the given noteNumber:
    for (int i = 0; i < total; i++) {
        Voice& v = voice[i];
        if (v.active && v.note == note) {
            v.env.noteOff();
            v.active = false;
        }
    }
}

void VoiceManager::setTune(float t, int osc) {
	for (int i = 0; i < total; i++) {
		voice[i].osc[osc].setTune(t);
	}
}

void VoiceManager::setLevel(float l, int osc) {
	for (int i = 0; i < total; i++) {
		voice[i].osc[osc].setLvl(l);
	}
}

void VoiceManager::setMorph(float f, int osc) {
	for (int i = 0; i < total; i++) {
		voice[i].osc[osc].setMorph(f);
	}
}

void VoiceManager::setShape(float f, int osc) {
	for (int i = 0; i < total; i++) {
		voice[i].osc[osc].setShape(f);
	}
}

void VoiceManager::setPW(float pw, int osc) {
	for (int i = 0; i < total; i++) {
		voice[i].osc[osc].setPW(pw);
	}
}

void VoiceManager::setFM(float fm) {
	for (int i = 0; i < total; i++) {
		voice[i].fm = fm;
	}
}

void VoiceManager::setMix(float m) {
	for (int i = 0; i < total; i++) {
		voice[i].mix = m;
	}
}

void VoiceManager::setCutoff(float c) {
	for (int i = 0; i < total; i++) {
		voice[i].filt.setCutoff(c);
	}
}

void VoiceManager::setResonance(float r) {
	for (int i = 0; i < total; i++) {
		voice[i].filt.setResonance(r);
	}
}

void VoiceManager::setEnvelope(float v, Envelope::stage s) {
	for (int i = 0; i < total; i++) {
		voice[i].env.setStage(v, s);
	}
}

float VoiceManager::process() {
	return voice[0].process() + voice[1].process() + voice[2].process() + voice[3].process();
}

Voice* VoiceManager::getFree() {
    Voice* freeVoice = NULL;
    for (int i = 0; i < total; i++) {
        if (!voice[i].active) {
            freeVoice = &(voice[i]);
            break;
        }
    }
    return freeVoice;
}