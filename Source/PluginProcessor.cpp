/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Week9tutorialAudioProcessor::Week9tutorialAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Week9tutorialAudioProcessor::~Week9tutorialAudioProcessor()
{
}

//==============================================================================
const juce::String Week9tutorialAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Week9tutorialAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Week9tutorialAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Week9tutorialAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Week9tutorialAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Week9tutorialAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Week9tutorialAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Week9tutorialAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Week9tutorialAudioProcessor::getProgramName (int index)
{
    return {};
}

void Week9tutorialAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void Week9tutorialAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Week9tutorialAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Week9tutorialAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    osc.setsampleRate(sampleRate);
    osc.setFrequency(220.0f);

    str.setsampleRate(sampleRate);
    str.setFrequency(220.0f);
    str.setLength(0.77f);
    str.setRadius(0.00051f);
    str.setT60(5.0f);
    str.setParameters();
    str.initGrid();
    
    
    inputForce.setDur(round(0.0008 * sampleRate));
    inputForce.setFamp(5.0f);
    std::vector<float> force = inputForce.halfHann();
    
    LFO.setsampleRate(sampleRate);
    LFO.setFrequency(1.0f);

}
void Week9tutorialAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    auto* leftChannel = buffer.getWritePointer(0);          // 0 for left channel
    auto* rightChannel = buffer.getWritePointer(1);         // 1 for right channel

    // Input Force
    int inputDuration = inputForce.getDur();
    std::vector<float> force = inputForce.halfHann();

    // ------------------------ Start DSP loop -----------------------------------------------
    for (int i = 0; i < numSamples; i++) {
        if (i < inputDuration) {
            str.setForce(force[i]);
        }
        else {
            str.setForce(0.0f);
        }
        
        leftChannel[i] =  str.process();
        rightChannel[i] = leftChannel[i];
    }

}

//==============================================================================
bool Week9tutorialAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Week9tutorialAudioProcessor::createEditor()
{
    return new Week9tutorialAudioProcessorEditor (*this);
}

//==============================================================================
void Week9tutorialAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Week9tutorialAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Week9tutorialAudioProcessor();
}
