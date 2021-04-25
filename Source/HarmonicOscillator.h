#pragma once
/**
 Class to create instances of a harmonic sine oscillator
 By setting the fundamental frequency, number of harmonics and strength,
 multiple sine tone harmonics can be created 

 Uses Oscillator.h objects to create sine tones
*/
#include<math.h>
#include<vector>
#include "Oscillator.h"


class HarmonicOscillator {
public:
	/*
	* 
	Sample value is sum of each harmonic and its corresponding gain
	Returns value of sample
	*/
	float process() {

		float sample = 0.0;

		for (int i = 0; i < numHarm; i++) {
			float gain = exp(-float(i + 1)/strength);   // Gain = exp(-n/strength) for nth harmonic
			sample += gain * harm[i].processSine();
		}
		return sample;
	}
	/*
	Sets frequency and sampleRate values for each harmonics
	*/
	void setFrequency(float freq) {
		fundFreq = freq;

		for (int i = 0; i < numHarm; i++) {
			harm.push_back(Oscillator());			
		}
		for (int i = 0; i < numHarm; i++) {
			harm[i].setsampleRate(sampleRate);
			harm[i].setFrequency(fundFreq * (i + 1));
		}
		
	}
	/** Returns fundamental frequency */
	float getFrequency() {
		return fundFreq;

	}
	/** Set and get sample rate */
	void setsampleRate(float SR) {
		sampleRate = SR;
	}
	float getsampleRate() {
		return sampleRate;
	}

	/** Set number of harmonics */
	void setNumHarm(int num) {
		numHarm = num;
	}
	/** Returns number of harmonics */
	float getNumHarm() {
		return numHarm;
	}

	/** Set strength of the oscillator. Uses an exponential formula to obtain gain*/
	void setStrength(float s) {
		strength = s;
	}
	/** Returns oscillator strength */
	float getStrength() {
		return strength;
	}


private:
	int numHarm = 5;                           // Number of harmonics
	float sampleRate = 44100;                  // Sample Rate
	float fundFreq;                            // Fundamental frequency
	float strength = 3;                        // Strength of hamonics(to set gain)

	std::vector<Oscillator> harm;              // Vector of oscillators to store as harmonics
	
	
};