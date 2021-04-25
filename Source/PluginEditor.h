/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Week9tutorialAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Week9tutorialAudioProcessorEditor (Week9tutorialAudioProcessor&);
    ~Week9tutorialAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Week9tutorialAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Week9tutorialAudioProcessorEditor)
};
