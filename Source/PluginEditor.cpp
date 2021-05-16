/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Pass_InputAudioProcessorEditor::Pass_InputAudioProcessorEditor(Pass_InputAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), audioTree(vts)
{
    setSize(450, 250);
    
    controlGain.setColour(0x1001400, juce::Colour::fromRGBA(0x00, 0x40, 0x00, 0x80));
    controlGain.setColour(0x1001700, juce::Colour::fromRGBA(0x00, 0x00, 0x00, 0x00));

    controlGain.setSliderStyle(juce::Slider::LinearHorizontal);
    //controlGain.addListener(this);
    controlGain.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 40, 20);
    addAndMakeVisible(controlGain);
    sliderAttachGain.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioTree, "controlGain_ID", controlGain));
    labelGain.setText(("Input Gain"), juce::dontSendNotification);
    labelGain.setFont(juce::Font("Slope Opera", 16, 0));
    labelGain.setColour(juce::Label::textColourId, juce::Colour::fromRGBA(0x40, 0x40, 0x80, 0xff));
    addAndMakeVisible(labelGain);
}


Pass_InputAudioProcessorEditor::~Pass_InputAudioProcessorEditor()
{
    sliderAttachGain.reset();
}

//==============================================================================
void Pass_InputAudioProcessorEditor::paint (juce::Graphics& g)
{    
    g.fillAll (juce::Colours::white);
 
    // set the current drawing colour to black
    g.setColour (juce::Colours::black);
 
    // set the font size and draw text to the screen
    g.setFont (15.0f);
    g.setFont(juce::Font("Slope Opera", 35.0f, 1));
    g.drawFittedText ("Pass Input", getLocalBounds(), juce::Justification::centred, 1);
}

void Pass_InputAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    controlGain.setBounds(140, getHeight() - 20, getWidth() - 140, 20);
    labelGain.setBounds(0, getHeight() - 20, 140, 20);
}
