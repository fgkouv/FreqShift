#ifndef NOISEOSCILLATOR_H_INCLUDED
#define NOISEOSCILLATOR_H_INCLUDED



#include "../JuceLibraryCode/JuceHeader.h"
#include <random>

//==============================================================================
/**
*/
class NoiseOscillator 
{
public:
    NoiseOscillator();

    float getNextSample();

    void setRange(float min, float max);

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<float> m_distribution;
};

#endif
