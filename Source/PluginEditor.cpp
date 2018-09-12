/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FreqShiftAudioProcessorEditor::FreqShiftAudioProcessorEditor (FreqShiftAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    addAndMakeVisible(&m_frequencySlider);
    addAndMakeVisible(&m_mixSlider);
    
    addAndMakeVisible(&m_frequencyLabel);
    addAndMakeVisible(&m_mixLabel);
    
    m_logoFont = Font(Typeface::createSystemTypefaceFor (BinaryData::Shkoder_1989_Light_otf, BinaryData::Shkoder_1989_Light_otfSize));
    m_quarejFont = Font(Typeface::createSystemTypefaceFor (BinaryData::SHPinscherRegular_otf, BinaryData::SHPinscherRegular_otfSize));
    

    m_frequencySlider.setName("Frequency");
    m_frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    m_frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    m_frequencySlider.setTextValueSuffix(" Hz");
    m_frequencySlider.addListener(this);
    m_freqShiftSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "freqshift", m_frequencySlider);
    
    
    m_mixSlider.setName("Mix");
    m_mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    m_mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    m_mixSlider.setNumDecimalPlacesToDisplay(2);
    m_mixSlider.addListener(this);
    m_mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "mix", m_mixSlider);

    m_frequencyLabel.setText("frequency shift", juce::NotificationType::dontSendNotification);
    m_frequencyLabel.setFont(m_logoFont);
    m_frequencyLabel.setJustificationType(juce::Justification::centred);
    
    
    m_mixLabel.setText("mix", juce::NotificationType::dontSendNotification);
    m_mixLabel.setFont(m_logoFont);
    m_mixLabel.setJustificationType(juce::Justification::centred);

}

FreqShiftAudioProcessorEditor::~FreqShiftAudioProcessorEditor()
{
}

//==============================================================================
void FreqShiftAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);

    g.setFont(m_logoFont);
    g.setFont(20.f);
    juce::Rectangle<int> titleBounds = juce::Rectangle<int>(0.25f * getWidth(), 10, getWidth() * 0.5f, 50);
    g.drawFittedText ("FREQSHIFT", titleBounds, Justification::centred, 1);
    
    g.setFont(m_quarejFont);
    g.setFont(15.f);
    juce::Rectangle<int> quarejBounds = juce::Rectangle<int>(0.8f * getWidth(), getHeight() - 50, getWidth() * 0.2f, 40);
    g.drawFittedText ("by QUA.REJ", quarejBounds, Justification::centred, 1);
}


void FreqShiftAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &m_frequencySlider)
    {
        processor.setFrequencyShift(slider->getValue());
    }
    
    if (slider == &m_mixSlider)
    {
        processor.setDryWetMix(slider->getValue());
    }
}

void FreqShiftAudioProcessorEditor::resized()
{
    m_frequencyLabel.setBounds(10, 0.35f * getHeight() - 40, 0.4f * getWidth(), 40);
    m_mixLabel.setBounds(0.6f * getWidth() - 10, 0.35f * getHeight() - 40, 0.4f * getWidth(), 40);
    
    
    m_frequencySlider.setBounds(10, 0.35f * getHeight(), 0.4f * getWidth(), 0.45f * getHeight());
    m_mixSlider.setBounds(0.6f * getWidth() - 10, 0.35f * getHeight(), 0.4f * getWidth(), 0.45f * getHeight());
    
}
