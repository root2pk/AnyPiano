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

    ///* Destructor*/
    //~Note() {
    //    //delete noteSampleCount;
    //    //delete forceSignal;
    //}
    /* Process function for note which adds samples from str.process for each string(based on some interval) and returns the sample*/
    float process() {
        float sample = 0.0f;

        for (int i = 0; i < numStrings; i++) {  

            if (sampleCount >= interval*i) {

                if (noteSampleCount[i] < durationInSamples) {
                    str[i]->setForce(forceSignal[noteSampleCount[i]]);
                }

                else {
                    str[i]->setForce(0.0f);
                }

                sample += str[i]->process();
                noteSampleCount[i]++;
            }
        }
        sampleCount++;
        return sample;
    }

    /* Sets the sample rate*/
    void setSampleRate(float samplerate) {
        sampleRate = samplerate;
    }

    /* Sets the parameters for each string*/
    void setStringParams(float frequencyinHz, float lengthInMetres, float radiusInMillimetres, float T60TimeInSeconds){
        freq = frequencyinHz;
        L = lengthInMetres;
        r = radiusInMillimetres/1000.0f;
        T60 = T60TimeInSeconds;
        for (int i = 0; i < numStrings; i++) {
            str[i]->setsampleRate(sampleRate);
            str[i]->setParameters(frequencyinHz + random.nextFloat(),L,r,T60);
            str[i]->initGrid();
        }
    }

    /* Sets the parameters for input force*/
    void setForceParameters(float durationInMilliseconds, float amplitudeInNewtons) {
        // Set duration of force signal in samples
        durationInSamples = round((durationInMilliseconds/1000.0f)*sampleRate);
        // Maximum amplitude of force signal
        famp = amplitudeInNewtons;
        // Set parameters
        inputForce.setDur(durationInSamples);
        inputForce.setFamp(famp);

        // Store forceSignal
        forceSignal = inputForce.fullHann();
    }

    /* Sets the number of strings in a note*/
    void setNumStrings(int number){
        numStrings = number;
        for (int i = 0; i < numStrings; i++) {
            str.push_back(new String);
        }
        noteSampleCount = new int[numStrings] {0};
    }

    int getNumString(){
        return numStrings;
    }

    /* Set interval between string excitation in milliseconds(set SampleRate first)*/
    void setInterval(float intervalInMilliseconds){
        interval = round((intervalInMilliseconds/1000.0)*sampleRate);
    }
    float getInterval(){
        return interval;
    }
private:

    // Members to pass on to String.h 
    float sampleRate;
    float freq;
    float L;
    float r;
    float T60;
    std::vector<String*> str;

    // Members to pass on to Input Force
    int durationInSamples;
    float famp;
    float *forceSignal;
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