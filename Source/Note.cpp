/*
==============================================================================

Note.cpp
Created: 3 May 2021 10:35:15pm
Author:  Ruthu

==============================================================================
*/

#include "Note.h"

float Note::process() {
    float sample = 0.0f;

    for (int i = 0; i < numStrings; i++) {

        if (sampleCount >= interval * i) {

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

void Note::setSampleRate(float samplerate) {
    sampleRate = samplerate;
}

void Note::setStringParams(float frequencyinHz, float lengthInMetres, float radiusInMillimetres, float T60TimeInSeconds) {
    freq = frequencyinHz;
    L = lengthInMetres;
    r = radiusInMillimetres / 1000.0f;
    T60 = T60TimeInSeconds;
    for (int i = 0; i < numStrings; i++) {
        str[i]->setsampleRate(sampleRate);
        str[i]->setParameters(frequencyinHz + random.nextFloat(), L, r, T60);
        str[i]->initGrid();
    }
}

void Note::setMaterial(float youngsModulus, float density) {
    for (int i = 0; i < numStrings; i++) {
        str[i]->setMaterial(youngsModulus, density);
    }
}

void Note::setInputOutput(float xi, float xo) {
    for (int i = 0; i < numStrings; i++) {
        str[i]->setExcCoordinates(xi, xo);
    }
}

void Note::setForceParameters(float durationInMilliseconds, float amplitudeInNewtons) {

    // Set duration of force signal in samples
    durationInSamples = round((durationInMilliseconds / 1000.0f) * sampleRate);
    // Maximum amplitude of force signal
    famp = amplitudeInNewtons;
    // Set parameters
    inputForce.setDur(durationInSamples);
    inputForce.setFamp(famp);

    // Store forceSignal
    forceSignal = inputForce.fullHann();
}

void Note::setNumStrings(int number) {
    numStrings = number;
    for (int i = 0; i < numStrings; i++) {
        str.push_back(new String);
    }
    noteSampleCount = new int[numStrings] {0};
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
