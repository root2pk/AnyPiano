/*
==============================================================================

Note.cpp
Created: 3 May 2021 
Author:  B119185

==============================================================================
*/

#include "Note.h"

float Note::process() {
    // Initialise sample
    float sample = 0.0f;

    // For each string, add sample
    for (int i = 0; i < numStrings; i++) {

        // Check if excitation time for each string has been reached and add to sample
        if (sampleCount >= interval * i) {

            // If the sample number is within the input force time, add input force
            if (stringSampleCount[i] < durationInSamples) {
                str[i]->setForce(forceSignal[stringSampleCount[i]]);
            }

            else {
                str[i]->setForce(0.0f);
            }
            // Obtain signal from i'th string and add to sample 
            sample += str[i]->process();
            // Sample count for string increases
            stringSampleCount[i]++;
        }
    }
    // Sample count for the note increases
    sampleCount++;

    return sample;
}

void Note::setSampleRate(float samplerate) {
    sampleRate = samplerate;
}

void Note::setStringParams(float frequencyinHz, float frequencyParam, float lengthInMetres, float radiusInMillimetres, float T60TimeInSeconds) {
    freq = frequencyinHz;
    L = lengthInMetres;
    r = radiusInMillimetres / 1000.0f;
    T60 = T60TimeInSeconds;
    float freqParam = frequencyParam;

    // Set parameters for each string with random frequency near note frequency
    for (int i = 0; i < numStrings; i++) {
        str[i]->setsampleRate(sampleRate);
        str[i]->setParameters(frequencyinHz + freqParam * (random.nextFloat()-0.5f), L, r, T60);
        str[i]->initGrid();
    }
}

void Note::setMaterial(float youngsModulus, float density) {
    // Set material for each string
    for (int i = 0; i < numStrings; i++) {
        str[i]->setMaterial(youngsModulus, density);
    }
}

void Note::setInputOutput(float xi, float xo) {
    // Set excitation coordinates for each string
    for (int i = 0; i < numStrings; i++) {
        str[i]->setExcCoordinates(xi, xo);
    }
}

void Note::setForceParameters(float durationInMilliseconds, float amplitudeInNewtons, bool choice) {

    // Set duration of force signal in samples
    durationInSamples = round((durationInMilliseconds / 1000.0f) * sampleRate);
    // Maximum amplitude of force signal
    famp = amplitudeInNewtons;
    // Set parameters
    inputForce.setDur(durationInSamples);
    inputForce.setFamp(famp);

    // Struck or Plucked
    excChoice = choice;

    // Store forceSignal
    if (excChoice == true) {
        forceSignal = inputForce.fullHann();           // Struck
    }
    else {
        forceSignal = inputForce.halfHann();           // Plucked
    }
}

void Note::setNumStrings(int number) {
    // Set number of strings for note and initialise that many instances of String objects
    numStrings = number;
    for (int i = 0; i < numStrings; i++) {
        str.push_back(new String);
    }
    stringSampleCount = new int[numStrings] {0};
}

int Note::getNumStrings() {
    return numStrings;
}

void Note::setInterval(float intervalInMilliseconds) {
    interval = round((intervalInMilliseconds / 1000.0) * sampleRate);
}

float Note::getInterval() {
    return interval;
}

