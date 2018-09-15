/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "HilbertShifter.h"
#include "WavetableOscillator.h"


//==============================================================================
/**
*/
class FreqShiftAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FreqShiftAudioProcessor();
    ~FreqShiftAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    void setDryWetMix(float newMixValue);
    void setFrequencyShift(float newFrequencyShift);
    
    void setLFORate(float newRate);
    void setLFODepth(float newDepth);
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    juce::AudioBuffer<float> m_processedBuffer;
    
    float m_dryWetMix { 1.f };
    
    std::vector<std::unique_ptr<HilbertShifter>> m_hilberFreqShifter;

    float m_lowpassCutoff { 5000.f };
    std::vector<std::unique_ptr<juce::IIRFilter>> m_lowpassFilter;
    juce::IIRCoefficients m_lowpassCoefficients;

    const int numStereoChannels { 2 };

    WavetableOscillator m_frequencyLFO;
    
    AudioProcessorValueTreeState m_parameters;
    
    float m_lfoDepth { 1.f };
    float m_freqShift { 0.f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreqShiftAudioProcessor)
};
