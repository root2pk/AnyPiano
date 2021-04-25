#pragma once
/*
Oscillator class

frequency and sampleRate for an Oscillator object can be set.

Using the various process() functions, a sample corresponding to the required oscillator type can be obtained.

*/
#include <iostream>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Oscillator {
public:
	/*
	Increments phase value by phaseDelta and returns phase
	*/
	float process() {
		phase += phaseDelta;

		if (phase > 1.0) {
			phase -= 1.0;
		}

		return phase;
	}
	/*
	Increments phase value by phaseDelta 
	Returns sine wave value of correpsonding phase
	*/
	float processSine() {
		float p = process();

		return sin(2 * M_PI * p);
	}
	/*
	Increments phase value by phaseDelta
	Returns square wave value of correpsonding phase
	*/
	float processSquare() {
		float p = process();

		float s = 0.5f;
		if (p > 0.5) {
			s = -0.5f;
		}
		return s;
	}
	/*
	Increments phase value by phaseDelta
	Returns triangle wave value of correpsonding phase
	*/
	float processTri() {
		float p = process();

		return (fabs(p - 0.5f) - 0.5f);
	}
	/*
	Increments phase value by phaseDelta
	Returns shifted triangle wave value of correpsonding phase ( 0 to 1)
	*/
	float processShiftTri() {
		float p = process();

		return fabs(p - 0.5) + 0.5;
	}

	/* 
	Increments phase value by phaseDelta
	Returns 1 if at the beginning on phase loop, othewise 0
	*/
	float processKroneckerDelta() {
		float p = process();

		if (p < 5 * phaseDelta) { return 1; }		
		else { return 0; }
	}
	/*
	Sets frequency value for oscillator and correspoding phaseDelta value
	*/
	void setFrequency(float freq) {
		frequency = freq;
		phaseDelta = frequency / sampleRate;
	}
	float getFrequency() {
		return frequency;
	}
	void setsampleRate(float SR) {
		sampleRate = SR;
	}
	float getsampleRate() {
		return sampleRate;
	}

private:
	float sampleRate = 44100;
	float phase = 0.0f;
	float frequency;
	float phaseDelta;
	
};

