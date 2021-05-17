/*
  ==============================================================================

    Synth.h
    Created: 30 Apr 2021 
    Author: B119185

    Class to control Synthesiser voices and Synthesiser sounds.
    Accesses Note.h to create instances of notes, and plays a sample-by-sample 
    output from Note.process()

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Note.h"

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
// Synthesiser Voice 

/*!
 @class SynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple SynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {}

    void init(float sampleRate) {
        
        /// Note
        note.setSampleRate(sampleRate);
        
        /// ADSR
        env.setSampleRate(sampleRate); 

    }

    /* */
    void setParamPointers(std::atomic<float>* T60In, std::atomic<float>* gainIn,
        std::atomic<float>* velCurveIn, std::atomic<float>* baseVelIn,
        std::atomic<float>* choiceIn,
        std::atomic<float>* EIn, std::atomic<float>* rhoIn){

        T60time = T60In;
        gain = gainIn;
        choice = choiceIn;

        baseVel = baseVelIn;
        velCurve = velCurveIn;

        E = EIn;
        rho = rhoIn;
    }

    void setNotePointers(std::atomic<float>* intervalIn, std::atomic<float>* freqParamIn, 
        std::atomic<float>* xiIn, std::atomic<float>* xoIn,
        std::atomic<float>* lengthParamIn, std::atomic<float>* radiusParamIn,
        std::atomic<float>* lim1In, std::atomic<float>* lim2In) {

        interval = intervalIn;
        freqParam = freqParamIn;

        xi = xiIn;
        xo = xoIn;

        lengthParam = lengthParamIn;
        radiusParam = radiusParamIn;

        lim1 = lim1In;
        lim2 = lim2In;
    }

    /* Set pointers for ADSR variable parameters*/
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

        // Number of strings in Note (based on lim1 and lim2)
        if (midiNoteNumber < int(*lim1)) {
            note.setNumStrings(1);
        }
        else if(midiNoteNumber < int(*lim2)) {
            note.setNumStrings(2);
        }
        else {
            note.setNumStrings(3);
        }

        bool excChoice;
        /// Struck or plucked
        if (*choice < 0.5f) {
            excChoice = true;       // Struck
        }
        else {
            excChoice = false;      // Plucked
        }

        // Set length and radius of strings of note
        float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) + 0.1443 * midiNoteNumber - 7.766;
        float length =  (*lengthParam) * (-0.019196429 * float(midiNoteNumber) + 1.815625);
        float radius = (*radiusParam) * (-2.08333e-03 * float(midiNoteNumber) + 0.62875);
        
        // Set Note properties
        note.setInterval(*interval);
        note.setMaterial((*E) * 1e9, *rho);
        note.setInputOutput(*xi, *xo);
        note.setStringParams(frequency, *freqParam, length, radius, *T60time);
        note.setForceParameters(3.0 - 2.0 * velocity, *baseVel + (*velCurve) * (2.0f * velocity - 0.5f), excChoice);

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
            /// ADSR variable parameters (variable while note is playing)
            juce::ADSR::Parameters envParams;
            envParams.attack = *attack;
            envParams.decay = *decay;
            envParams.sustain = *sustain;
            envParams.release = *release;
            env.setParameters(envParams);

            /// Gain value
            float G = *gain;

            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // Sample-by-sample DSP code here!

                // Get ADSR envelope value
                float envVal = env.getNextSample();

                // Get currentSample from note.process()
                float currentSample = envVal * note.process();

                // For each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by gain G 
                    outputBuffer.addSample(chan, sampleIndex, currentSample * G);
                }

                // Check if the end of the note has been reached
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
     Can this voice play a sound. 

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

    bool playing = false;
    bool ending = false;

    /// Note object
    Note note;

    /// Variable Parameters
    std::atomic<float>* T60time;                                    // T60 time
    std::atomic<float>* gain;                                       // Gain
    std::atomic<float>* choice;                                     // Float value for struck or plucked

    // Note properties
    std::atomic<float>* interval;                                   // Interval between string strikes in note
    std::atomic<float>* freqParam;                                  // Frequency randomising scaler

    // Velocity
    std::atomic<float>* velCurve;                                   // Velocity scaling
    std::atomic<float>* baseVel;                                    // Least value for input force (N)

    // Material Properties
    std::atomic<float>* E;                                          // Young's Modulus
    std::atomic<float>* rho;                                        // Density

    // Excitation properties
    std::atomic<float>* xi;                                         // Coordinate of excitation
    std::atomic<float>* xo;                                         // Coordinate of output


    // Physical properties
    std::atomic<float>* lengthParam;                                // Parameter to adjust length of strings                                  
    std::atomic<float>* radiusParam;                                // Parameter to adjust radius of strings

    std::atomic<float>* lim1;                                       // MIDI range till which note has 1 strings
    std::atomic<float>* lim2;                                       // MIDI range till which note has 2 strings

    /// ADSR
    juce::ADSR env;
    std::atomic<float>* attack;
    std::atomic<float>* decay;
    std::atomic<float>* sustain;
    std::atomic<float>* release;

};
