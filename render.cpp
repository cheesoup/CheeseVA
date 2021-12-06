/*
CheeseVA: DIY Phase Distortion Synthesizer
By Christopher Carin

TODO:
- Parameter interpolation
- Figure out modulation matrix
- Stereo-ify everything
- Distortion effect
- Reverb Effect (If I have time/CPU left)
- Arpegiator/Sequencer (If I have time/CPU left)
- Figure out LED/LCD screen and hardware!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

#include <Bela.h>
#include <stdio.h>
#include <vector>
#include <libraries/Scope/Scope.h>
#include <libraries/Midi/Midi.h>
#include <libraries/math_neon/math_neon.h>

#include "global.h"
#include "voicemanager.h"
#include "delay.h"
#include "param.h"

// Debug
bool print_on = true;
bool scope_on = false;
const char* port = "hw:0,0";

// Objects for MIDI and oscilloscope
Midi midi;
Scope scope;

// Globals
float* sr = new float;
float* inertia = new float;
float* hpOut = new float;

VoiceManager* voices = new VoiceManager;
VDelay* vdelay = new VDelay;
Param* delTime = new Param;

float delFB = 0;
float delLength;
float delLvl = 0;

// MIDI CC
void gMidiCC(MidiChannelMessage message)
{
	int controller = message.getDataByte(0);
	int value = message.getDataByte(1);
	switch(controller)
	{
		case 8:
		{
			float tune = (float)value / 127.0;
			tune = powf_neon(2.0, tune);
			voices->setTune(tune, 0);
			if (print_on) rt_printf("tune0: %f, midi: %d", tune, value);
			break;
		}
		case 9:
		{
			float tune = (float)value / 127.0;
			tune = powf_neon(2.0, tune);
			voices->setTune(tune, 1);
			if (print_on) rt_printf("tune1: %f, midi: %d", tune, value);
			break;
		}
		case 10:
		{
			float morph = (float)value / 127.0;
			if (print_on) rt_printf("morph: %f, midi: %d", morph, value);
			voices->setMorph(morph, 0);
			break;
		}
		case 11:
		{
			float morph = (float)value / 127.0;
			if (print_on) rt_printf("morph: %f, midi: %d", morph, value);
			voices->setMorph(morph, 1);
			break;
		}
		case 12:
		{
			float shape = (float)value / 127.0;
			if (print_on) rt_printf("shape: %f, midi: %d", shape, value);
			voices->setShape(shape, 0);
			break;
		}
		case 13:
		{
			float shape = (float)value / 127.0;
			if (print_on) rt_printf("shape: %f, midi: %d", shape, value);
			voices->setShape(shape, 1);
			break;
		}
		case 14:
		{
			float pw = (float)value / 127.0;
			voices->setPW(pw, 0);
			if (print_on) rt_printf("pw: %f, midi: %d", pw, value);
			break;
		}
		case 15:
		{
			float pw = (float)value / 127.0;
			voices->setPW(pw, 1);
			if (print_on) rt_printf("pw: %f, midi: %d", pw, value);
			break;
		}
		case 16:
		{
			float fm = (float)value / 127.0 * 2.0;
			voices->setFM(fm);
			if (print_on) rt_printf("fm: %f, midi: %d", fm, value);
			break;
		}
		case 17:
		{
			float mix = (float)value / 127.0;
			voices->setMix(mix);
			if (print_on) rt_printf("mix: %f, midi: %d", mix, value);
			break;
		}
		case 18:
		{
			float cutoff = powf(2.0, ((float)value - 69.0) / 12.0) * 440.0;
			voices->setCutoff(cutoff);
			if (print_on) rt_printf("freq: %f, midi: %d", cutoff, value);
			break;
		}
		case 19:
		{
			float res = ((float)value / 127.0) * ((float)value / 127.0);
			voices->setResonance(res);
			if (print_on) rt_printf("res: %f, midi: %d", res, value);
			break;
		}
		case 20:
		{
			float atk = ((float)value / 127.0) * 0.5;
			voices->setEnvelope(atk, Envelope::ATTACK);
			if (print_on) rt_printf("atk: %f, midi: %d", atk, value);
			break;
		}
		case 21:
		{
			float dec = ((float)value / 127.0) * 2.0;
			voices->setEnvelope(dec, Envelope::DECAY);
			if (print_on) rt_printf("dec: %f, midi: %d", dec, value);
			break;
		}
		case 22:
		{
			float sus = ((float)value / 127.0);
			voices->setEnvelope(sus, Envelope::SUSTAIN);
			if (print_on) rt_printf("sus: %f, midi: %d", sus, value);
			break;
		}
		case 23:
		{
			float rel = ((float)value / 127.0) * 2.0;
			voices->setEnvelope(rel, Envelope::RELEASE);
			if (print_on) rt_printf("rel: %f, midi: %d", rel, value);
			break;
		}
		case 24:
		{
			delLength = (float)value / 127.0;
			delTime->setTarget(delLength);
			if (print_on) rt_printf("delLength: %f, midi: %d", delTime->getTarget(), *sr);
			break;
		}
		case 25:
		{
			delFB = ((float)value / 127.0) * 0.999;
			if (print_on) rt_printf("delFB: %f, midi: %d", delFB, value);
			break;
		}
		case 26:
		{
			delLvl = ((float)value / 127.0);
			if (print_on) rt_printf("delLvl: %f, midi: %d", delLvl, value);
			break;
		}
	}
}

inline void gMidiOn(MidiChannelMessage message)
{
	int note = message.getDataByte(0);
	int vel = message.getDataByte(1);
	voices->on(note, vel);
}

inline void gMidiOff(MidiChannelMessage message)
{
	int note = message.getDataByte(0);
	voices->off(note);
}

void midiMessageCallback(MidiChannelMessage message, void* arg)
{
	switch(message.getType())
	{
		case kmmControlChange:
			gMidiCC(message);
			break;
		case kmmNoteOn:
			if ((bool)message.getDataByte(1)) gMidiOn(message);
			else gMidiOff(message);
			break;
		default:
			break;
	}
	rt_printf("\n");
}

bool setup(BelaContext *context, void *userData)
{
	*inertia = 0.125;
	*sr = context->audioSampleRate;
	*hpOut = exp(-2.0*M_PI*20/44100.0);
	
	voices->setup(context->audioSampleRate);
	vdelay->setup(sr, 2.0);
	delTime->setup(sr, inertia);
	
	delTime->setCurrent(0.5);
	
	// Initialize Scope
	if (scope_on) scope.setup(1, context->audioSampleRate);
	
	// Initialize MIDI
	midi.readFrom(port);
	midi.writeTo(port);
	midi.enableParser(true);
	midi.getParser()->setCallback(midiMessageCallback, (void*) port);
	
	return true;
}

void render(BelaContext *context, void *userData)
{	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		// Process parameters
		delTime->process();
		
		// Get sample
		float out = voices->process();
		out += vdelay->process(out, delTime->getCurrent(), delFB) * delLvl;
		out *= 0.5;
		
		clip(&out, 1, -1);
		float hp = onepole(out, *hpOut);
		out -= hp;
		
		// Write sample to block
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		
		// Write sample to scope
		if (scope_on) scope.log(out);
	}
}

void cleanup(BelaContext *context, void *userData)
{
	delete hpOut;
	delete sr;
	delete voices;
	delete vdelay;
}

