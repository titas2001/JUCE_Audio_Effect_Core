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
class Pass_InputAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Pass_InputAudioProcessorEditor (Pass_InputAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~Pass_InputAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Pass_InputAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& audioTree;
    juce::Slider controlGain;
    juce::Label labelGain;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachGain;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pass_InputAudioProcessorEditor)
};
