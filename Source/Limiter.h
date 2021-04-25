#pragma once

#include <assert.h>
/*
Limiter class
Limits the audio between an upper limit and lower limit to prevent clipping/distortion

Preset limits are -1 and +1
User changeable through setLimit()

*/


class Limiter {
public:

	/*
	Limit the sample between speciied upper and lower limits
	-1 to -1 if not specified 
	
	*/
	float processSample(float sample) {
		if (sample > UpLimit) {return UpLimit;}
		if (sample < LoLimit) {return LoLimit;}
		return sample;
	}

	/* Processes the limiter for an array of samples in stereo */
	void processStereo(float* const left, float* const right, const int numSamples) noexcept
	{
		jassert(left != nullptr && right != nullptr);

		for (int i = 0; i < numSamples; ++i)
		{
			if (left[i] > UpLimit) { left[i] = UpLimit; }
			if (left[i] < LoLimit) { left[i] = LoLimit; }
			if (right[i] > UpLimit) { right[i] = UpLimit; }
			if (right[i] < LoLimit) { right[i] = LoLimit; }
		}
	}

	/* Processes the limiter for an array of mono samples*/
	void processMono(float* const samples, const int numSamples) noexcept
	{
		jassert(samples != nullptr);

		for (int i = 0; i < numSamples; ++i)
		{
			if (samples[i] > UpLimit) { samples[i] = UpLimit; }
			if (samples[i] < LoLimit) { samples[i] = LoLimit; }
		}
	}

	/*
	Set custom upper and lower limits
	
	setLimit(upper limit, lower limit)

	*/
	void setLimit(float lolim, float uplim) {
		jassert(lolim < uplim);
		UpLimit = uplim;
		LoLimit = lolim;

	}

private:
	float UpLimit = 1.0;
	float LoLimit = -1.0;
};
