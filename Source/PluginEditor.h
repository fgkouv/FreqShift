/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FreqShiftAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    FreqShiftAudioProcessorEditor (FreqShiftAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~FreqShiftAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void sliderValueChanged (Slider* slider) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FreqShiftAudioProcessor& processor;
    
    Font m_logoFont;
    Font m_quarejFont;
    
    juce::Label m_frequencyLabel;
    juce::Label m_mixLabel;
    
    juce::Slider m_frequencySlider;
    juce::Slider m_mixSlider;
    
    juce::Label m_lfoRateLabel;
    juce::Label m_lfoDepthLabel;
    
    juce::Slider m_lfoRateSlider;
    juce::Slider m_lfoDepthSlider;
    
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_freqShiftSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_mixSliderAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_lfoRateSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_lfoDepthSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreqShiftAudioProcessorEditor)
};
