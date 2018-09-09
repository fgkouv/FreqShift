/*
  ==============================================================================

    HilbertShifter.h
    Created: 4 Sep 2018 9:08:23pm
    Author:  F.GK

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BlackmanWindow.h"

#include <math.h>       /* fmod */


class HilbertShifter
{
public:
    HilbertShifter();
    ~HilbertShifter();
    
    void setFrequencyShift(float newShift);
    
    void prepareToPlay(float sampleRate, int newBlockSize);
    void processBlock(float* inputBlock, int numSamples);
    
private:
    int m_blockSize;
    float m_sampleRate {48000};
    float m_frequencyShift {20};
    
    std::vector<float> m_filter1DelayLine;
    std::vector<float> m_filter2DelayLine;
    
    
    std::vector<float> m_hilbert_B1_FilterCoefs;
    std::vector<float> m_hilbert_B2_FilterCoefs;
    
    const int numberFilter1Coefs { 31 };
    const int numberFilter2Coefs { 16 };
};