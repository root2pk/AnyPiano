/*
  ==============================================================================

    Note.h
    Created: 29 Apr 2021 
    Author:  B119185
        
    Class to create an instance of a 'note', which is a combination of strings.
    The number of strings is based on where on the keyboard the note lies.

    The strings are combined by delaying the excitation time of each string 
    by a set interval, and the frequencies of each string are changed using a 
    sclaed random number, to prevent phase addition and artificial sounds.

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
    void setStringParams(float frequencyinHz, float frequencyParam, float lengthInMetres, float radiusInMillimetres, float T60TimeInSeconds);

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
    float sampleRate;                               // Sample Rate
    float freq;                                     // Frequency of the note
    float L;                                        // Length of the strings of the note
    float r;                                        // Radius of the strings of the note
    float T60;                                      // T60 time of the note

    // Vector of string objects
    std::vector<String*> str;                       // vector of string objects

    // Input Force parameters
    int durationInSamples;                          // duration of input force in samples
    float famp;                                     // Max amplitude of input force (N)
    float *forceSignal;                             // Force signal 
    bool excChoice;                                 // Type of excitation(plucked/struck)
    Hann inputForce;                                // Object to return force signal        

    // Random
    juce::Random random;                            // Random object

    // Note properties
    int numStrings;                                 // Number of strings in the note
    float interval;                                 // Excitation interval between strings (in samples)

    // Counters to keep track of how many samples have passed for each string and the note in total
    int sampleCount = 0;                            
    int *stringSampleCount;
};