/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Pass_InputAudioProcessor::Pass_InputAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    audioTree(*this, nullptr, juce::Identifier("PARAMETERS"),
                        { std::make_unique<juce::AudioParameterFloat>("controlGain_ID","ControlGain",juce::NormalisableRange<float>(0.01, 1.0, 0.01),0.5)
                        }),
                    lowPassFilter(juce::dsp::IIR::Coefficients< float >::makeLowPass((48000.0 * 4.0), 20000.0))
#endif
{
    oversampling.reset(new juce::dsp::Oversampling<float>(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true));
    audioTree.addParameterListener("controlGain_ID", this);
    controlledGain = 0.5;
}

Pass_InputAudioProcessor::~Pass_InputAudioProcessor()
{
    oversampling.reset();
}

//==============================================================================
const juce::String Pass_InputAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Pass_InputAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Pass_InputAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Pass_InputAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Pass_InputAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pass_InputAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Pass_InputAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Pass_InputAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Pass_InputAudioProcessor::getProgramName (int index)
{
    return {};
}

void Pass_InputAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Pass_InputAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling->reset();
    oversampling->initProcessing(static_cast<size_t> (samplesPerBlock));

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate * 4;
    spec.maximumBlockSize = samplesPerBlock * 3;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    controlledGain = 0.5;
}

void Pass_InputAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Pass_InputAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Pass_InputAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    /***************************************************************************************/

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    //juce::dsp::AudioBlock<float> blockInput(buffer);
    //juce::dsp::AudioBlock<float> blockOutput = oversampling->processSamplesUp(blockInput);

    //updateFilter(0);
    //lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(blockOutput));
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            vin = buffer.getSample(channel, sample);
            vout = vin*controlledGain;
            buffer.setSample(channel, sample, vout);
        }
    }    
    //updateFilter(0);
    //lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(blockOutput));

    //oversampling->processSamplesDown(blockInput);

    //updateFilter(1);
    //lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(blockInput));

}
void Pass_InputAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    //Parameters update  when sliders moved
    if (parameterID == "controlGain_ID") {
        controlledGain = newValue;
    }
}
//==============================================================================
bool Pass_InputAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Pass_InputAudioProcessor::createEditor()
{
    return new Pass_InputAudioProcessorEditor (*this, audioTree);
}
void Pass_InputAudioProcessor::updateFilter(bool realFreq)
{
    double frequency;

    if (realFreq) frequency = 48e3;
    else frequency = 48e3 * 4;

    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(frequency, 20000.0);
}
//==============================================================================
void Pass_InputAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Pass_InputAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pass_InputAudioProcessor();
}
