#ifndef WAVETABLEOSCILLATOR_H_INCLUDED
#define WAVETABLEOSCILLATOR_H_INCLUDED


#include "NoiseOscillator.h"
#include "emmintrin.h"
#include <array>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Wavetables.h"

enum WaveShapes
{
    Sinusoidal = 0,
    Triangle,
    Sawtooth,
    ReverseSawtooth,
    Square,
    Exponential,
    Logarithm,
    RC,
    Tangent,
    HalfRectifiedSin,
    FullRectifiedSin,
    SquareRoot,
    PowerTwo,
    TriangleIntegral,
    Sinc,
    Staircase,
    SinStaircase,
    Hann,
    Gauss,
    BandlimitedSignal,
    BandlimitedImpulseTrain,
    BandlimitedTriangle,
    BandlimitedSawtooth,
    BandlimitedReverseSawtooth,
    BandlimitedSquare,
    Noise,
    NumberOfWaveshapes
};

class WavetableOscillator
{
public:
    
    WavetableOscillator();
    ~WavetableOscillator();

    enum Interpolation
    {
        NONE,
        LINEAR,
        CUBIC
    };
    
    
    enum Polarity
    {
        monopolar,
        bipolar
    };
    
    void setSampleRate (double newSampleRate);
    void setFrequency(float newFreq);
    void setWaveShape(WaveShapes newShape);
    void setInterpolationToUse(Interpolation newSetting);
    
    float getFrequency() { return frequencyInHz; }
    WaveShapes getWaveShape() { return shape; }
    
    void resetPhase() { phase = 0.0f; }
    void resetPhase(float offsetInRadians);
    
    void getNextOptimisedBlock(float* ioBuffer);
    float getNextSample();
    
    void processBuffer(juce::AudioSampleBuffer& buffer);
    
    void setPolaritySetting(Polarity newSetting);
    
    
    // Getters for synths trivial tests
    float getSampleRate()
    {
        return sampleRate;
    }
    
private:
    
    float updateWithoutInterpolation();
    float updateBipolarValueWithoutInterpolation();

    void updateWithoutInterpolationSSEOptimised(float* ioBuffer);
    void updateBipolarValueWithoutInterpolationSSEOptimised(float* ioBuffer);

    
    float updateWithLinearInterpolation();
    float updateBipolarValueWithLinearInterpolation();
    void updateWithLinearInterpolationSSEOptimised(float* ioBuffer);
    void updateBipolarValueWithLinearInterpolationSSEOptimised(float* ioBuffer);
    
    float updateWithCubicInterpolation();
    float updateBipolarValueWithCubicInterpolation();
    void updateWithCubicInterpolationSSEOptimised(float* ioBuffer);
    void updateBipolarValueWithCubicInterpolationSSEOptimised(float* ioBuffer);
    
    float* m_sseBuffer;
    int* m_indexes;
    float* m_interpolationCoefficients;
    
    float* m_sseInterp0;
    float* m_sseInterp1;
    float* m_sseInterp2;
    float* m_sseInterp3;
    float* m_sseTableValues;
    
    static const int SSE_BLOCK_SIZE { 4 };
    
    float phase         {  0.0f };
    float increment     {  1.0f };
    float frequencyInHz {   440 };
    float sampleRate    { 44100 };

    int tableSize       { 32 };
    
    NoiseOscillator m_noiseOscillator;
    
    Polarity m_polaritySetting { bipolar };
    WaveShapes shape { Sinusoidal };
    Interpolation interpolationSetting { LINEAR };
};



#endif 
