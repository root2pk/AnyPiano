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
    float process() {
        float sample = 0.0f;

        for (int i = 0; i < numStrings; i++) {  

            if (sampleCount >= interval*i) {

                if (noteSampleCount[i] < durationInSamples) {
                    str[i].setForce(forceSignal[noteSampleCount[i]]);
                }

                else {
                    str[i].setForce(0.0f);
                }

                sample += str[i].process();
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
            str[i].setsampleRate(sampleRate);
            str[i].setFrequency(freq + random.nextFloat());
            str[i].setLength(L);
            str[i].setRadius(r);
            str[i].setT60(T60);
            str[i].setParameters();
            str[i].initGrid();
        }
    }

    /* Sets the parameters for input force*/
    void setForceParameters(float durationInMilliseconds, float amplitudeInNewtons) {
        durationInSamples = round((durationInMilliseconds/1000.0f)*sampleRate);
        famp = amplitudeInNewtons;
        inputForce.setDur(durationInSamples);
        inputForce.setFamp(famp);

        forceSignal = inputForce.fullHann();
    }


    void setNumStrings(int number){
        numStrings = number;
        for (int i = 0; i < numStrings; i++) {
            str.push_back(String());
            noteSampleCount.push_back(0);
        }
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
    std::vector<String> str;

    // Members to pass on to Input Force
    int durationInSamples;
    float famp;
    std::vector<float> forceSignal;
    Hann inputForce;

    // Random
    juce::Random random;

    // Note properties
    int numStrings;
    float interval;

    // Counter
    int sampleCount = 0;
    std::vector<int> noteSampleCount;
};