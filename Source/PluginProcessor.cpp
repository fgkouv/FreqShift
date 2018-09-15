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
, m_parameters (*this, nullptr)
{
    
    m_parameters.createAndAddParameter ("freqshift",                        // parameter ID
                                        "Frequency Shift",                      // parameter name
                                        " Hz",                                           // parameter label (suffix)
                                        NormalisableRange<float> (-500.f, 500.f, 5.f, 1.f),  // range
                                        0.f,                                            // default value
                                        nullptr,
                                        nullptr);
    
    m_parameters.createAndAddParameter ("mix",                       // parameter ID
                                        "Mix",                     // parameter name
                                        "",                                           // parameter label (suffix)
                                        NormalisableRange<float> (0.f, 1.f, 0.01f),  // range
                                        1.f,                                            // default value
                                        nullptr,
                                        nullptr);
    
    m_parameters.createAndAddParameter ("lfoRate",                       // parameter ID
                                        "LFO rate",                     // parameter name
                                        "",                                           // parameter label (suffix)
                                        NormalisableRange<float> (1.f, 200.f, 1.f),  // range
                                        20.f,                                            // default value
                                        nullptr,
                                        nullptr);
    
    m_parameters.createAndAddParameter ("lfoDepth",                       // parameter ID
                                        "LFO depth",                     // parameter name
                                        "",                                           // parameter label (suffix)
                                        NormalisableRange<float> (0.f, 100.f, 1.f),  // range
                                        100.f,                                            // default value
                                        nullptr,
                                        nullptr);
    
    m_parameters.state = ValueTree (Identifier ("FreqShift"));
    
    
    for (int i=0;i<numStereoChannels;++i)
    {
        m_hilberFreqShifter.push_back(std::make_unique<HilbertShifter>());
        m_lowpassFilter.push_back(std::make_unique<juce::IIRFilter>());
    }
    
    m_frequencyLFO.setPolaritySetting(WavetableOscillator::Polarity::monopolar);
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
    return 1;
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
    
    m_lowpassCoefficients = IIRCoefficients::makeLowPass(sampleRate, m_lowpassCutoff);
    
    m_frequencyLFO.setSampleRate(sampleRate);
    
    for (int i=0;i<numStereoChannels;++i)
    {
        m_lowpassFilter[i]->setCoefficients(m_lowpassCoefficients);
        m_lowpassFilter[i]->reset();
        
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
    m_freqShift = newFrequencyShift;
    
    for (int i=0;i < m_hilberFreqShifter.size();++i)
    {
        m_hilberFreqShifter[i]->setFrequencyShift(newFrequencyShift);
    }
}


void FreqShiftAudioProcessor::setLFORate(float newRate)
{
    m_frequencyLFO.setFrequency(newRate);
}


void FreqShiftAudioProcessor::setLFODepth(float newDepth)
{
    m_lfoDepth = newDepth;
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
        
        m_lowpassFilter[channel]->processSamples(m_processedBuffer.getWritePointer(channel), numSamples);
        
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
    return new FreqShiftAudioProcessorEditor (*this, m_parameters);
}

//==============================================================================
void FreqShiftAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (m_parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void FreqShiftAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (m_parameters.state.getType()))
        {
            m_parameters.state = ValueTree::fromXml (*xmlState);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreqShiftAudioProcessor();
}
