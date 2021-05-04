/*
  ==============================================================================

    Synth.h
    Created: 30 Apr 2021 2:28:16pm
    Author:  Ruthu

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Note.h"
#include "Oscillator.h"

// ===========================
// ===========================
// SOUND
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel(int) override { return true; }
};




// =================================
// =================================
// Synthesiser Voice - Synth code goes in here

/*!
 @class SynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple SynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {}

    void init(float sampleRate) {
        
        //Note
        note.setSampleRate(sampleRate);
        
        /// ADSR
        env.setSampleRate(sampleRate); 

    }

    void setParamPointers(std::atomic<float>* T60In, std::atomic<float>* gainIn,
        std::atomic<float>* velCurveIn, std::atomic<float>* choiceIn,
        std::atomic<float>* EIn, std::atomic<float>* rhoIn){

        T60time = T60In;
        gain = gainIn;
        choice = choiceIn;

        velCurve = velCurveIn;

        E = EIn;
        rho = rhoIn;
    }

    void setNotePointers(std::atomic<float>* intervalIn, std::atomic<float>* intTimeIn,
        std::atomic<float>* xiIn, std::atomic<float>* xoIn,
        std::atomic<float>* lengthParamIn, std::atomic<float>* radiusParamIn,
        std::atomic<float>* lim1In, std::atomic<float>* lim2In) {

        interval = intervalIn;
        intTime = intTimeIn;

        xi = xiIn;
        xo = xoIn;

        lengthParam = lengthParamIn;
        radiusParam = radiusParamIn;

        lim1 = lim1In;
        lim2 = lim2In;
    }

    void setADSRPointers(std::atomic<float>* A, std::atomic<float>* D, std::atomic<float>* S, std::atomic<float>* R) {
        attack = A;
        decay = D;
        sustain = S;
        release = R;
    }
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;

        // Number of strings in Note
        if (midiNoteNumber < int(*lim1)) {
            note.setNumStrings(1);
        }
        else if(midiNoteNumber < int(*lim2)) {
            note.setNumStrings(2);
        }
        else {
            note.setNumStrings(3);
        }

        // Struck or plucked
        if (*choice < 0.5f) {
            excChoice = true;       // Struck
        }
        else {
            excChoice = false;      // Plucked
        }

        // Set length and radius of strings of note
        float length =  (*lengthParam) * (-0.019196429 * float(midiNoteNumber) + 1.815625);
        float radius = (*radiusParam) * (-2.08333e-03 * float(midiNoteNumber) + 0.62875);
        
        // Set Note properties
        note.setInterval(*interval);
        note.setMaterial((*E) * 1e9, *rho);
        note.setInputOutput(*xi, *xo);
        note.setStringParams(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber), length, radius, *T60time);
        note.setForceParameters(*intTime, 15.0f + (*velCurve) * (2.0f * velocity - 0.5f), excChoice);

        /// ADSR
        env.reset();
        env.noteOn();

    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {   
        if (allowTailOff) {
            env.noteOff();
            ending = true;
        }
        else {
            clearCurrentNote();
            playing = false;
        }
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here

     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {   

        if (playing) // check to see if this voice should be playing
        {
            /// ADSR variable parameters
            juce::ADSR::Parameters envParams;
            envParams.attack = *attack;
            envParams.decay = *decay;
            envParams.sustain = *sustain;
            envParams.release = *release;
            env.setParameters(envParams);

            float G = *gain;

            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code

                float envVal = env.getNextSample();

                float currentSample = envVal * note.process();

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by gain G 
                    outputBuffer.addSample(chan, sampleIndex, currentSample * G);
                }

                if (ending) {
                    if (envVal < 0.001f) {
                        clearCurrentNote();
                        playing = false;
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of SynthSound
     */
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;

    /// Note object
    Note note;
    bool excChoice;
    std::atomic<float>* T60time;

    /// Variable Parameters
    std::atomic<float>* gain;
    std::atomic<float>* choice;

    std::atomic<float>* interval;
    std::atomic<float>* velCurve;
    std::atomic<float>* intTime;

    std::atomic<float>* E;
    std::atomic<float>* rho;
    std::atomic<float>* xi;
    std::atomic<float>* xo;

    std::atomic<float>* lengthParam;
    std::atomic<float>* radiusParam;

    std::atomic<float>* lim1;
    std::atomic<float>* lim2;

    /// ADSR
    juce::ADSR env;
    std::atomic<float>* attack;
    std::atomic<float>* decay;
    std::atomic<float>* sustain;
    std::atomic<float>* release;

};
