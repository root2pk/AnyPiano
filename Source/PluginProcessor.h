/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Hann.h"
#include "Note.h"
#include "Synth.h"


//==============================================================================
/**
*/
class PluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    /// Audio Processor value parameters
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* T60time;
    std::atomic<float>* attack;
    std::atomic<float>* decay;
    std::atomic<float>* sustain;
    std::atomic<float>* release;
    std::atomic<float>* gain;
    std::atomic<float>* interval;
    std::atomic<float>* velCurve;
    std::atomic<float>* intTime;

    std::atomic<float>* youngsModulus;
    std::atomic<float>* density;

    std::atomic<float>* xi;
    std::atomic<float>* xo;

    /// Synth parameters
    juce::Synthesiser synth;
    int voiceCount = 16;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
