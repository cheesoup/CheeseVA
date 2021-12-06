#include <Bela.h>
#include <cmath>
#include <libraries/math_neon/math_neon.h>

#include "envelope.h"
#include "global.h"


Envelope::Envelope(float* sr) {
	setup(sr);
}

void Envelope::setup(float* sr) {
	SR = sr;
	vel = 0.0;
	lvl = 0.0;
	current = 0;
	mult = 1.0;
	curStage = OFF;
	
	adsr[OFF] = 0.0;
	adsr[ATTACK] = 0.01;
	adsr[DECAY] = 1.0;
	adsr[SUSTAIN] = 0.125;
	adsr[RELEASE] = 2.0;
}

void Envelope::noteOn(float v) {
	vel = v;
	go(ATTACK);
}

void Envelope::noteOff() {
	go(RELEASE);
}

void Envelope::setStage(float v, Envelope::stage s) {
	switch(s) {
		default:
			v = fmaxf(v, min_time);
			break;
		case OFF:
			return;
		case SUSTAIN:
			v = fmaxf(v, min_lvl);
	}
	adsr[s] = v;
}

float Envelope::process() {
	if ((curStage != OFF) && (curStage != SUSTAIN)) {
		if (current <= 0) {
			stage next = static_cast<stage>((curStage + 1) % STAGETOTAL);
			go(next);
		}
		lvl *= mult;
		current--;
	}
	clip(&lvl, 1.0, 0.0);
	return lvl;
}

void Envelope::calcMult(float start, float end, unsigned int length) {
	mult = 1.0 + (logf_neon(end) - logf_neon(start)) / (float)length;
}

void Envelope::go(stage next) {
	curStage = next;
	if ((curStage == OFF) || (curStage == SUSTAIN)) current = 0;
	else current = adsr[curStage] * *SR; // length of curStage * 44100
    switch (curStage) {
        case OFF:
			lvl = 0.0;
			mult = 1.0;
            break;
        case ATTACK:
			lvl = lvl < min_lvl ? min_lvl : lvl;
			calcMult(lvl, vel, current);
            break;
        case DECAY:
			lvl = vel;
			calcMult(lvl, fmax(adsr[SUSTAIN], min_lvl), current);
            break;
        case SUSTAIN:
			lvl = adsr[SUSTAIN];
			mult = 1.0;
            break;
        case RELEASE:
			calcMult(lvl, min_lvl, current);
            break;
        default:
            break;
    }
}

Envelope::stage Envelope::getStage() const {
	return curStage;
}
