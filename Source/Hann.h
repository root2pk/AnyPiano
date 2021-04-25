#pragma once
/**
 Class to create instances of a harmonic sine oscillator
 By setting the fundamental frequency, number of harmonics and strength,
 multiple sine tone harmonics can be created

 Uses Oscillator.h objects to create sine tones
*/
#include<math.h>
#include<vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Hann{
public:
	/*
	*
	Sample value is sum of each harmonic and its corresponding gain
	Returns value of sample
	*/
	std::vector<float> fullHann() {

		//float sample = 0.0;

		//for (int i = 0; i < numHarm; i++) {
		//	float gain = exp(-float(i + 1) / strength);   // Gain = exp(-n/strength) for nth harmonic
		//	sample += gain * harm[i].processSine();
		//}
		//return sample;

		std::vector<float> f;
		for (int i = 0; i < dur; i++) {
			f.push_back(famp * 0.5 * (1 - cos(2 * M_PI * float(i / dur))));
		}
		return f;
	}

	std::vector<float> halfHann() {

		std::vector<float> f;
		for (int i = 0; i < dur; i++) {
			f.push_back(famp * 0.5 * (1 - cos(M_PI * float(i / dur))));
		}

		return f;
	}
	/* Set peak amplitude of force signal */
	void setFamp(float f) {
		famp = f;
	}
	float getFamp() {
		return famp;
	}

	/* Set duration of force signal in samples*/
	void setDur(int d) {
		dur = d;
	}
	int getDur() {
		return dur;
	}

private:
	float famp;                         //  Peak amplitude of input (N)
	int dur;                            // Duration of input in samples

};