/*
  ==============================================================================

    Note.h
    Created: 29 Apr 2021 4:44:50pm
    Author:  Ruthu
        
    Class to create an instance of a 'note', which is a combination of strings.
    The number of strings is based on where on the keyboard the note lies.

  ==============================================================================
*/

#pragma once

#include "String.h"
#include "Hann.h"
#include <vector>
#include <JuceHeader.h>

class Note {
public:

    /* Process function for note which adds samples from str.process for each string(based on some interval) and returns the sample*/
    float process();

    /* Sets the sample rate*/
    void setSampleRate(float samplerate);

    /* Sets the parameters for each string*/
    void setStringParams(float frequencyinHz, float lengthInMetres, float radiusInMillimetres, float T60TimeInSeconds);

    /* Sets the string material properties (in SI units)*/
    void setMaterial(float youngsModulus, float density);

    /* Sets the input and output coordinates (0-1) */
    void setInputOutput(float xi, float xo);

    /* Sets the parameters for input force*/
    void setForceParameters(float durationInMilliseconds, float amplitudeInNewtons, bool choice);

    /* Sets the number of strings in a note*/
    void setNumStrings(int number);

    /* Returns the number of strings in a note*/
    int getNumStrings();

    /* Set interval between string excitation in milliseconds(set SampleRate first)*/
    void setInterval(float intervalInMilliseconds);
    float getInterval();

private:

    // Members to pass on to String.h 
    float sampleRate;
    float freq;
    float L;
    float r;
    float T60;

    // Vector of string objects
    std::vector<String*> str;

    // Input Force parameters
    int durationInSamples;
    float famp;
    float *forceSignal;
    bool excChoice;
    Hann inputForce;

    // Random
    juce::Random random;

    // Note properties
    int numStrings;
    float interval;

    // Counter
    int sampleCount = 0;
    int *noteSampleCount;
};