/*
  ==============================================================================

    MySynth.h
    Created: 21 Apr 2021 4:27:57pm
    Author:  Ruthu Prem Kumar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "Oscillator.h"
#include "HarmonicOscillator.h"
#include "Note.h"
#include "Hann.h"

// ===========================
// ===========================
// SOUND
class MySynthSound : public juce::SynthesiserSound
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
 @class MySynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple MySynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice : public juce::SynthesiserVoice
{
public:
    MySynthVoice() {}

    // Set sample rate for each voice
    void init(float sampleRate) {
        //osc.setsampleRate(sampleRate);
        str.setsampleRate(sampleRate);
        env.setSampleRate(sampleRate);
        inputForce.setDur(0.0008*sampleRate);
        inputForce.setFamp(5.0f);


        // -------------------  ADSR Parameters  ----------------------------
        ADSR::Parameters envParams;
        envParams.attack = 2.5f;
        envParams.decay = 0.25f;
        envParams.sustain = 0.5f;
        envParams.release = 1.0f;

        env.setParameters(envParams);

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
        // Set oscialltor frequency based on midi note number
        float freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        float length = -0.01919 * float(midiNoteNumber) + 1.81563;
        float radius = -2.0833e-6 * float(midiNoteNumber) + 6.29e-4;

        //osc.setFrequency(freq);
        str.setFrequency(freq);
        str.setLength(length);
        str.setRadius(radius);
        str.initGrid();
        str.setParameters();

        //// Input Force
        //std::vector<float> force = inputForce.halfHann();
        //int n = 0;

        // ADSR
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
            // DSP LOOP (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                float envVal = env.getNextSample();

                // Current sample               
                float currentSample = str.process() * envVal;

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample * 0.2);
                }

                // Check if ADSR release has ended
                if (ending)
                {
                    if (envVal < 0.0001f) {
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
     @return sound cast as a pointer to an instance of MySynthSound
     */
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here

    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;            // Boolean for checking note ending
    
    /// A random object
    juce::Random random;

    // String Object
    Note str;

    // Hann Input Force
    Hann inputForce;

    // Oscillator object
    HarmonicOscillator osc;

    // ADSR object
    ADSR env;


};