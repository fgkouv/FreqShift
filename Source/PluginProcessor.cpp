/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FreqShiftAudioProcessor::FreqShiftAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{    
    for (int i=0;i<numStereoChannels;++i)
    {
        m_hilberFreqShifter.push_back(std::make_unique<HilbertShifter>());
    }

}

FreqShiftAudioProcessor::~FreqShiftAudioProcessor()
{
}

//==============================================================================
const String FreqShiftAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FreqShiftAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FreqShiftAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FreqShiftAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FreqShiftAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FreqShiftAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FreqShiftAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FreqShiftAudioProcessor::setCurrentProgram (int index)
{
}

const String FreqShiftAudioProcessor::getProgramName (int index)
{
    return {};
}

void FreqShiftAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FreqShiftAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_processedBuffer.setSize(numStereoChannels, samplesPerBlock);
    
    for (int i=0;i<numStereoChannels;++i)
    {
        m_hilberFreqShifter[i]->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void FreqShiftAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FreqShiftAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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



void FreqShiftAudioProcessor::setDryWetMix(float newMixValue)
{
    m_dryWetMix = newMixValue;
}

void FreqShiftAudioProcessor::setFrequencyShift(float newFrequencyShift)
{
    for (int i=0;i < m_hilberFreqShifter.size();++i)
    {
        m_hilberFreqShifter[i]->setFrequencyShift(newFrequencyShift);
    }
}


void FreqShiftAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    int numSamples = m_processedBuffer.getNumSamples();
    
    for (int channel = 0; channel < numStereoChannels; ++channel)
    {
        const float* inputBuffer = buffer.getReadPointer(channel);
        float* outputBuffer = buffer.getWritePointer(channel);

        
        FloatVectorOperations::copy(m_processedBuffer.getWritePointer(channel), inputBuffer, numSamples);
        m_hilberFreqShifter[channel]->processBlock(m_processedBuffer.getWritePointer(channel), numSamples);
        
        const float* shiftedBuffer = m_processedBuffer.getReadPointer(channel);
        for (int i=0; i<numSamples; ++i)
        {
            outputBuffer[i] = m_dryWetMix * shiftedBuffer[i] + (1.f - m_dryWetMix) * inputBuffer[i];
        }
    }
    
}

//==============================================================================
bool FreqShiftAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FreqShiftAudioProcessor::createEditor()
{
    return new FreqShiftAudioProcessorEditor (*this);
}

//==============================================================================
void FreqShiftAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FreqShiftAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreqShiftAudioProcessor();
}
