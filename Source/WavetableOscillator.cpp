#include "WavetableOscillator.h"

#define M_PI 3.14159265359


WavetableOscillator::WavetableOscillator()
{
    tableSize = Wavetables::sinTable.size();
    
    m_noiseOscillator.setRange(-0.5, 0.5);
    
    m_sseBuffer = new float[SSE_BLOCK_SIZE];
    m_indexes = new int[SSE_BLOCK_SIZE];
    m_interpolationCoefficients = new float[SSE_BLOCK_SIZE];
    
    m_sseInterp0 = new float[SSE_BLOCK_SIZE];
    m_sseInterp1 = new float[SSE_BLOCK_SIZE];
    m_sseInterp2 = new float[SSE_BLOCK_SIZE];
    m_sseInterp3 = new float[SSE_BLOCK_SIZE];
    
    m_sseTableValues = new float[SSE_BLOCK_SIZE];
    
    for (int i = 0; i < SSE_BLOCK_SIZE; ++i)
    {
        m_sseBuffer[i] = 0.f;
        m_interpolationCoefficients[i] = 0.f;
        m_indexes[i] = 0;
        
        m_sseTableValues[i] = 0.f;
        m_sseInterp0[i] = 0.f;
        m_sseInterp1[i] = 0.f;
        m_sseInterp2[i] = 0.f;
        m_sseInterp3[i] = 0.f;

    }

}

WavetableOscillator::~WavetableOscillator()
{
    delete[] m_sseBuffer;
    delete[] m_indexes;
    delete[] m_interpolationCoefficients;
    
    delete[] m_sseInterp0;
    delete[] m_sseInterp1;
    delete[] m_sseInterp2;
    delete[] m_sseInterp3;
    
    delete[] m_sseTableValues;

}


void WavetableOscillator::setSampleRate (double newSampleRate)
{
    if (newSampleRate != 0)
    {
        sampleRate = newSampleRate;
        increment = static_cast<float>(tableSize) * (frequencyInHz / sampleRate);
        jassert(increment >= 0);
    }
}

void WavetableOscillator::setFrequency(float newFreq)
{
    frequencyInHz = newFreq;
    
    increment = static_cast<float>(tableSize) * (frequencyInHz / sampleRate);
}

void WavetableOscillator::setWaveShape(WaveShapes newShape)
{
    shape = newShape;
}

void WavetableOscillator::setInterpolationToUse(Interpolation newSetting)
{
    interpolationSetting = newSetting;
}


void WavetableOscillator::setPolaritySetting(Polarity newSetting)
{
    m_polaritySetting = newSetting;
}


void WavetableOscillator::resetPhase(float offsetInRadians)
{
    while (offsetInRadians < 0)
    {
        offsetInRadians += 2 * M_PI;
    }
    while (offsetInRadians > (2 * M_PI))
    {
        offsetInRadians -= 2 * M_PI;
    }
    
    phase = (offsetInRadians / (2 * M_PI)) * tableSize;
}


void WavetableOscillator::getNextOptimisedBlock(float* ioBuffer)
{
    if ( shape == WaveShapes::Noise)
    {
        for (int sample = 0; sample < SSE_BLOCK_SIZE; ++sample)
        {
            ioBuffer[sample] = m_noiseOscillator.getNextSample();
        }
    }
    else
    {

        if ( m_polaritySetting == bipolar )
        {
            switch (interpolationSetting)
            {
                case NONE:
                    updateBipolarValueWithoutInterpolationSSEOptimised(ioBuffer);
                    break;
                case LINEAR:
                    updateBipolarValueWithLinearInterpolationSSEOptimised(ioBuffer);
                    break;
                case CUBIC:
                    updateBipolarValueWithCubicInterpolationSSEOptimised(ioBuffer);
                    break;
            }
        }
        else
        {
            switch (interpolationSetting)
            {
                case NONE:
                    updateWithoutInterpolationSSEOptimised(ioBuffer);
                    break;
                case LINEAR:
                    updateWithLinearInterpolationSSEOptimised(ioBuffer);
                    break;
                case CUBIC:
                    updateWithCubicInterpolationSSEOptimised(ioBuffer);
                    break;
            }
        }
        
        
        
        //juce::FloatVectorOperations::copy(ioBuffer, m_sseBuffer, 4);
        
    }
    
    
    //return m_sseBuffer;
}



float WavetableOscillator::getNextSample()
{
    if ( shape == WaveShapes::Noise)
    {
        return m_noiseOscillator.getNextSample();
    }
    else
    {
        if ( m_polaritySetting == bipolar )
        {
            switch (interpolationSetting)
            {
                case NONE:
                    return updateBipolarValueWithoutInterpolation();
                    break;
                case LINEAR:
                    return updateBipolarValueWithLinearInterpolation();
                    break;
                case CUBIC:
                    return updateBipolarValueWithCubicInterpolation();
                    break;
            }
        }
        else
        {
            switch (interpolationSetting)
            {
                case NONE:
                    return updateWithoutInterpolation();
                    break;
                case LINEAR:
                    return updateWithLinearInterpolation();
                    break;
                case CUBIC:
                    return updateWithCubicInterpolation();
                    break;
            }
        }
    }
}

void WavetableOscillator::processBuffer(juce::AudioSampleBuffer& buffer)
{
    if (interpolationSetting == NONE)
    {
        for (int chan = 0; chan < buffer.getNumChannels(); ++chan)
        {
            float* chanPointer = buffer.getWritePointer(chan);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                chanPointer[i] = updateWithoutInterpolation();
            }
        }
    }
    else if (interpolationSetting == LINEAR)
    {
        for (int chan = 0; chan < buffer.getNumChannels(); ++chan)
        {
            float* chanPointer = buffer.getWritePointer(chan);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                chanPointer[i] = updateWithLinearInterpolation();
            }
        }
    }
    else
    {
        for (int chan = 0; chan < buffer.getNumChannels(); ++chan)
        {
            float* chanPointer = buffer.getWritePointer(chan);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                chanPointer[i] = updateWithCubicInterpolation();
            }
        }
    }
}

float WavetableOscillator::updateWithoutInterpolation()
{
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    
    if (i < 0)
    {
        i = 0;
    }
    
    
    if (shape == Sinusoidal)
    {
        return Wavetables::sinTable[i];
    }
    else
    {
        return 0.0f;
    }
}



void WavetableOscillator::updateWithoutInterpolationSSEOptimised(float *ioBuffer)
{
    tableSize = Wavetables::sinTable.size(); //all tables must be the same size
    
    switch (shape)
    {
        case Sinusoidal:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::sinTable[m_indexes[index]];
            }
            
            break;
            
        case Triangle:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::triangleTable[m_indexes[index]];
            }
            
            break;
            
        case Sawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::sawtoothTable[m_indexes[index]];
            }
            
            break;
            
        case ReverseSawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTable[m_indexes[index]];
            }
            
            break;
            
        case Square:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::squareTable[m_indexes[index]];
            }
            
            break;
    }
    
    __m128 table = _mm_load_ps(m_sseTableValues);
    _mm_store_ps(ioBuffer, table);
}


void WavetableOscillator::updateBipolarValueWithoutInterpolationSSEOptimised(float* ioBuffer)
{
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
    
    switch (shape)
    {
        case Sinusoidal:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::sinTableBipolar[m_indexes[index]];
            }
            
            break;
            
        case Triangle:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::triangleTableBipolar[m_indexes[index]];
            }
            
            break;
            
        case Sawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::sawtoothTableBipolar[m_indexes[index]];
            }
            
            break;
            
        case ReverseSawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTableBipolar[m_indexes[index]];
            }
            
            break;
            
        case Square:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                
                m_sseTableValues[index] = Wavetables::squareTableBipolar[m_indexes[index]];
            }
            
            break;
    }
    
    
    __m128 table = _mm_load_ps(m_sseTableValues);
    _mm_store_ps(ioBuffer, table);
}


float WavetableOscillator::updateBipolarValueWithoutInterpolation()
{
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    
    if (i < 0)
    {
        i = 0;
    }
    
    
    if (shape == Sinusoidal)
    {
        return Wavetables::sinTableBipolar[i];
    }
    else
    {
        return 0.0f;
    }
}

              
float WavetableOscillator::updateWithLinearInterpolation()
{
    
    tableSize = Wavetables::sinTable.size(); //all tables must be the same size
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    
    if (i < 0)
    {
        i = 0;
    }
    
    float alpha = phase - static_cast<float>(i);

    float value = 0.0;
    
    switch (shape)
    {
    case Sinusoidal: 
        value = Wavetables::sinTable[i] + Wavetables::sinTableInterp0[i] * alpha; 
        break;
    case Triangle:   
        value = Wavetables::triangleTable[i] + Wavetables::triangleTableInterp0[i] * alpha;
        break;
    case Sawtooth:   
        value = Wavetables::sawtoothTable[i] + Wavetables::sawtoothTableInterp0[i] * alpha;
        break;
    case ReverseSawtooth:   
        value = Wavetables::reverseSawtoothTable[i] + Wavetables::reverseSawtoothTableInterp0[i] * alpha;
        break;
    case Square:   
        value = Wavetables::squareTable[i] + Wavetables::squareTableInterp0[i] * alpha;
        break;
            /*
    case Exponential:   
        value = Wavetables::exponentialTable[i] + Wavetables::exponentialTableInterp0[i] * alpha;
        break;
    case Logarithm:   
        value = Wavetables::logarithmTable[i] + Wavetables::logarithmTableInterp0[i] * alpha;
        break;
    case RC:   
        value = Wavetables::rcChargeDischargeTable[i] + Wavetables::rcChargeDischargeTableInterp0[i] * alpha;
        break;
    case Tangent:   
        value = Wavetables::tangentTable[i] + Wavetables::tangentTableInterp0[i] * alpha; 
        break;
    case HalfRectifiedSin:   
        value = Wavetables::halfRectifiedSinTable[i] + Wavetables::halfRectifiedSinTableInterp0[i] * alpha;
        break;
    case FullRectifiedSin:   
        value = Wavetables::fullRectifiedSinTable[i] + Wavetables::fullRectifiedSinTableInterp0[i] * alpha;
        break;
    case SquareRoot:   
        value = Wavetables::squareRootTable[i] + Wavetables::squareRootTableInterp0[i] * alpha;
        break;
    case PowerTwo:
        value = Wavetables::powerTwoTable[i] + Wavetables::powerTwoTableInterp0[i] * alpha;
        break;
    case TriangleIntegral:
        value = Wavetables::triangleIntegralTable[i] + Wavetables::triangleIntegralTableInterp0[i] * alpha;
        break;
    case Sinc:   
        value = Wavetables::sincTable[i] + Wavetables::sincTableInterp0[i] * alpha;
        break;
    case Staircase:   
        value = Wavetables::staircaseTable[i] + Wavetables::staircaseTableInterp0[i] * alpha;
        break;
    case SinStaircase:   
        value = Wavetables::sinStaircaseTable[i] + Wavetables::sinStaircaseTableInterp0[i] * alpha;
        break;
    case Hann:   
        value = Wavetables::hannTable[i] + Wavetables::hannTableInterp0[i] * alpha; 
        break;
    case Gauss:   
        value = Wavetables::gaussTable[i] + Wavetables::gaussTableInterp0[i] * alpha; 
        break;
    case BandlimitedSignal:   
        value = Wavetables::bandlimitedSignalTable[i] + Wavetables::bandlimitedSignalTableInterp0[i] * alpha;
        break;
    case BandlimitedImpulseTrain:   
        value = Wavetables::bandlimitedImpulseTable[i] + Wavetables::bandlimitedImpulseTableInterp0[i] * alpha;
        break;
    case BandlimitedTriangle:   
        value = Wavetables::bandlimitedTriangleTable[i] + Wavetables::bandlimitedTriangleTableInterp0[i] * alpha;
        break;
    case BandlimitedSawtooth:   
        value = Wavetables::bandlimitedSawtoothTable[i] + Wavetables::bandlimitedSawtoothTableInterp0[i] * alpha;
        break;
    case BandlimitedReverseSawtooth:   
        value = Wavetables::bandlimitedReverseSawtoothTable[i] + Wavetables::bandlimitedReverseSawtoothTableInterp0[i] * alpha;
        break;
    case BandlimitedSquare:   
        value = Wavetables::bandlimitedSquareTable[i] + Wavetables::bandlimitedSquareTableInterp0[i] * alpha;
        break;
             */
    default:        
        value = Wavetables::sinTable[i] + Wavetables::sinTableInterp0[i] * alpha;
        break;
    }

    return value;
}
              


float WavetableOscillator::updateBipolarValueWithLinearInterpolation()
{
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    

    if (i < 0)
    {
        i = 0;
    }
    
    float alpha = phase - static_cast<float>(i);
    
    float value = 0.0;
    
    switch (shape)
    {
        case Sinusoidal:
            value = Wavetables::sinTableBipolar[i] + Wavetables::sinTableInterp0Bipolar[i] * alpha;
            break;
        case Triangle:
            value = Wavetables::triangleTableBipolar[i] + Wavetables::triangleTableInterp0Bipolar[i] * alpha;
            break;
        case Sawtooth:
            value = Wavetables::sawtoothTableBipolar[i] + Wavetables::sawtoothTableInterp0Bipolar[i] * alpha;
            break;
        case ReverseSawtooth:
            value = Wavetables::reverseSawtoothTableBipolar[i] + Wavetables::reverseSawtoothTableInterp0Bipolar[i] * alpha;
            break;
        case Square:
            value = Wavetables::squareTableBipolar[i] + Wavetables::squareTableInterp0Bipolar[i] * alpha;
            break;
        default:
            value = Wavetables::sinTableBipolar[i] + Wavetables::sinTableInterp0Bipolar[i] * alpha;
            break;
    }
    
    return value;
}




void WavetableOscillator::updateBipolarValueWithCubicInterpolationSSEOptimised(float* ioBuffer)
{
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
   
    switch (shape)
    {
        case Sinusoidal:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                int i =  static_cast<int>(phase);
                
                m_indexes[index] = i;
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sinTableBipolar[i];
                
                m_sseInterp1[index] = Wavetables::sinTableInterp1Bipolar[i];
                m_sseInterp2[index] = Wavetables::sinTableInterp2Bipolar[i];
                m_sseInterp3[index] = Wavetables::sinTableInterp3Bipolar[i];
            }
            
            break;
            
        case Triangle:
           
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::triangleTableBipolar[m_indexes[index]];
                
                m_sseInterp1[index] = Wavetables::triangleTableInterp1Bipolar[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::triangleTableInterp2Bipolar[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::triangleTableInterp3Bipolar[m_indexes[index]];
            }
            
            break;
            
        case Sawtooth:
       
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sawtoothTableBipolar[m_indexes[index]];
                
                m_sseInterp1[index] = Wavetables::sawtoothTableInterp1Bipolar[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::sawtoothTableInterp2Bipolar[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::sawtoothTableInterp3Bipolar[m_indexes[index]];
            }
            
            break;
            
        case ReverseSawtooth:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTableBipolar[m_indexes[index]];
                
                m_sseInterp1[index] = Wavetables::reverseSawtoothTableInterp1Bipolar[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::reverseSawtoothTableInterp2Bipolar[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::reverseSawtoothTableInterp3Bipolar[m_indexes[index]];
            }
            break;
            
        case Square:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::squareTableBipolar[m_indexes[index]];
                
                m_sseInterp1[index] = Wavetables::squareTableInterp1Bipolar[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::squareTableInterp2Bipolar[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::squareTableInterp3Bipolar[m_indexes[index]];
            }
            break;
            
    }

    __m128 coefficients = _mm_load_ps(m_interpolationCoefficients);
    __m128 table = _mm_load_ps(m_sseTableValues);
    __m128 interp1 = _mm_load_ps(m_sseInterp1);
    __m128 interp2 = _mm_load_ps(m_sseInterp2);
    __m128 interp3 = _mm_load_ps(m_sseInterp3);
    
    
    __m128 firstProduct = _mm_mul_ps(interp1, coefficients);
    __m128 firstSum = _mm_add_ps(firstProduct, interp2);
    __m128 secondProduct = _mm_mul_ps(firstSum, coefficients);
    __m128 secondSum = _mm_add_ps(secondProduct, interp3);
    
    _mm_store_ps(ioBuffer, _mm_add_ps(table, _mm_mul_ps(secondSum, coefficients)));

}



void WavetableOscillator::updateWithCubicInterpolationSSEOptimised(float* ioBuffer)
{
    tableSize = Wavetables::sinTable.size(); //all tables must be the same size
    
    switch (shape)
    {
        case Sinusoidal:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sinTableBipolar[m_indexes[index]];
                m_sseInterp1[index] = Wavetables::sinTableInterp1[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::sinTableInterp2[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::sinTableInterp3[m_indexes[index]];
            }
            
            break;
        case Triangle:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::triangleTableBipolar[m_indexes[index]];
                m_sseInterp1[index] = Wavetables::triangleTableInterp1[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::triangleTableInterp2[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::triangleTableInterp3[m_indexes[index]];
            }
            
            break;
        case Sawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sawtoothTableBipolar[m_indexes[index]];
                m_sseInterp1[index] = Wavetables::sawtoothTableInterp1[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::sawtoothTableInterp2[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::sawtoothTableInterp3[m_indexes[index]];
            }
            
            break;
        case ReverseSawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTableBipolar[m_indexes[index]];
                m_sseInterp1[index] = Wavetables::reverseSawtoothTableInterp1[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::reverseSawtoothTableInterp3[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::reverseSawtoothTableInterp3[m_indexes[index]];
            }
            break;
            
        case Square:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::squareTableBipolar[m_indexes[index]];
                m_sseInterp1[index] = Wavetables::squareTableInterp1[m_indexes[index]];
                m_sseInterp2[index] = Wavetables::squareTableInterp3[m_indexes[index]];
                m_sseInterp3[index] = Wavetables::squareTableInterp3[m_indexes[index]];
            }
            
            break;
    }
    
    __m128 coefficients = _mm_load_ps(m_interpolationCoefficients);
    __m128 table = _mm_load_ps(m_sseTableValues);
    __m128 interp1 = _mm_load_ps(m_sseInterp1);
    __m128 interp2 = _mm_load_ps(m_sseInterp2);
    __m128 interp3 = _mm_load_ps(m_sseInterp3);
    
    
    __m128 firstProduct = _mm_mul_ps(interp1, coefficients);
    __m128 firstSum = _mm_add_ps(firstProduct, interp2);
    __m128 secondProduct = _mm_mul_ps(firstSum, coefficients);
    __m128 secondSum = _mm_add_ps(secondProduct, interp3);
    
    _mm_store_ps(ioBuffer, _mm_add_ps(table, _mm_mul_ps(secondSum, coefficients)));
}

void WavetableOscillator::updateBipolarValueWithLinearInterpolationSSEOptimised(float* ioBuffer)
{
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
    
    switch (shape)
    {
        case Sinusoidal:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sinTableBipolar[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::sinTableInterp0Bipolar[m_indexes[index]];
            }

            break;
            
        case Triangle:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sinTableBipolar[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::sinTableInterp0Bipolar[m_indexes[index]];
            }

            break;
            
        case Sawtooth:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sawtoothTableBipolar[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::sawtoothTableInterp0Bipolar[m_indexes[index]];
            }

            
            break;
            
        case ReverseSawtooth:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTableBipolar[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::reverseSawtoothTableInterp1Bipolar[m_indexes[index]];
            }
            break;
            
        case Square:
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::squareTableBipolar[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::squareTableInterp1Bipolar[m_indexes[index]];
            }
            
            break;
    }
    
    
    __m128 coefficients = _mm_load_ps(m_interpolationCoefficients);
    __m128 table = _mm_load_ps(m_sseTableValues);
    __m128 interp0 = _mm_load_ps(m_sseInterp0);
    
    _mm_store_ps(ioBuffer, _mm_add_ps(table, _mm_mul_ps(interp0, coefficients)));
}


void WavetableOscillator::updateWithLinearInterpolationSSEOptimised(float* ioBuffer)
{
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
    
    switch (shape)
    {
        case Sinusoidal:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sinTable[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::sinTableInterp0[m_indexes[index]];
            }
            
            break;
            
        case Triangle:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::triangleTable[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::triangleTableInterp0[m_indexes[index]];
            }
            
            break;
            
        case Sawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::sawtoothTable[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::sawtoothTableInterp0[m_indexes[index]];
            }
            
            break;
            
        case ReverseSawtooth:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::reverseSawtoothTable[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::reverseSawtoothTableInterp0[m_indexes[index]];
            }
            
            break;
            
        case Square:
            
            for (int index = 0; index < SSE_BLOCK_SIZE; ++index)
            {
                phase = fmod((phase + increment), tableSize);
                
                m_indexes[index] =  static_cast<int>(phase);
                
                if ( m_indexes[index] < 0 )
                {
                    m_indexes[index] = 0;
                }
                
                m_interpolationCoefficients[index] = phase - static_cast<float>(m_indexes[index]);
                
                m_sseTableValues[index] = Wavetables::squareTable[m_indexes[index]];
                m_sseInterp0[index] = Wavetables::squareTableInterp0[m_indexes[index]];
            }
            
            break;
    }
    
    __m128 coefficients = _mm_load_ps(m_interpolationCoefficients);
    __m128 table = _mm_load_ps(m_sseTableValues);
    __m128 interp0 = _mm_load_ps(m_sseInterp0);
    
    _mm_store_ps(ioBuffer, _mm_add_ps(table, _mm_mul_ps(interp0, coefficients)));
}


float WavetableOscillator::updateWithCubicInterpolation()
{
    
    tableSize = Wavetables::sinTable.size(); //all tables must be the same size
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    
    if (i < 0)
    {
        i = 0;
    }
    
    float alpha = phase - static_cast<float>(i);

    

    /* //remember to wrap around!!!
     dtable1[i] = (3.f*(table[i]-table[i+1])-table[i-1]+table[i+2])/2.f
     dtable2[i] = 2.f*table[i+1]+table[i-1]-(5.f*table[i]+table[i+2])/2.f
     dtable3[i] = (table[i+1]-table[i-1])/2.f
     */

    float value;
    switch (shape)
    {
    case Sinusoidal:
        value = ((Wavetables::sinTableInterp1[i] * alpha + Wavetables::sinTableInterp2[i]) * alpha + Wavetables::sinTableInterp3[i]) * alpha + Wavetables::sinTable[i];
        break;
    case Triangle:
        value =  ((Wavetables::triangleTableInterp1[i] * alpha + Wavetables::triangleTableInterp2[i]) * alpha + Wavetables::triangleTableInterp3[i]) * alpha + Wavetables::triangleTable[i];		
        break;
    case Sawtooth:
        value = ((Wavetables::sawtoothTableInterp1[i] * alpha + Wavetables::sawtoothTableInterp2[i]) * alpha + Wavetables::sawtoothTableInterp3[i]) * alpha + Wavetables::sawtoothTable[i];
        break;
    case ReverseSawtooth:
        value = ((Wavetables::reverseSawtoothTableInterp1[i] * alpha + Wavetables::reverseSawtoothTableInterp2[i]) * alpha + Wavetables::reverseSawtoothTableInterp3[i]) * alpha + Wavetables::reverseSawtoothTable[i];
        break;
    case Square:
        value = ((Wavetables::squareTableInterp1[i] * alpha + Wavetables::squareTableInterp2[i]) * alpha + Wavetables::squareTableInterp3[i]) * alpha + Wavetables::squareTable[i];
        break;
            /*
    case Exponential:
        value = ((Wavetables::exponentialTableInterp1[i] * alpha + Wavetables::exponentialTableInterp2[i]) * alpha + Wavetables::exponentialTableInterp3[i]) * alpha + Wavetables::exponentialTable[i];
        break;
    case Logarithm:
        value = ((Wavetables::logarithmTableInterp1[i] * alpha + Wavetables::logarithmTableInterp2[i]) * alpha + Wavetables::logarithmTableInterp3[i]) * alpha + Wavetables::logarithmTable[i];
        break;
    case RC:
        value = ((Wavetables::rcChargeDischargeTableInterp1[i] * alpha + Wavetables::rcChargeDischargeTableInterp2[i]) * alpha + Wavetables::rcChargeDischargeTableInterp3[i]) * alpha + Wavetables::rcChargeDischargeTable[i];
        break;
    case Tangent:
        value = ((Wavetables::tangentTableInterp1[i] * alpha + Wavetables::tangentTableInterp2[i]) * alpha + Wavetables::tangentTableInterp3[i]) * alpha + Wavetables::tangentTable[i];
        break;
    case HalfRectifiedSin:
        value = ((Wavetables::halfRectifiedSinTableInterp1[i] * alpha + Wavetables::halfRectifiedSinTableInterp2[i]) * alpha + Wavetables::halfRectifiedSinTableInterp3[i]) * alpha + Wavetables::halfRectifiedSinTable[i];
        break;
    case FullRectifiedSin:
        value = ((Wavetables::fullRectifiedSinTableInterp1[i] * alpha + Wavetables::fullRectifiedSinTableInterp2[i]) * alpha + Wavetables::fullRectifiedSinTableInterp3[i]) * alpha + Wavetables::fullRectifiedSinTable[i];
        break;
    case SquareRoot:
        value = ((Wavetables::squareRootTableInterp1[i] * alpha + Wavetables::squareRootTableInterp2[i]) * alpha + Wavetables::squareRootTableInterp3[i]) * alpha + Wavetables::squareRootTable[i];
        break;
    case PowerTwo:
        value = ((Wavetables::powerTwoTableInterp1[i] * alpha + Wavetables::powerTwoTableInterp2[i]) * alpha + Wavetables::powerTwoTableInterp3[i]) * alpha + Wavetables::powerTwoTable[i];
        break;
    case TriangleIntegral:
        value = ((Wavetables::triangleIntegralTableInterp1[i] * alpha + Wavetables::triangleIntegralTableInterp2[i]) * alpha + Wavetables::triangleIntegralTableInterp3[i]) * alpha + Wavetables::triangleIntegralTable[i];
        break;
    case Sinc:
        value = ((Wavetables::sincTableInterp1[i] * alpha + Wavetables::sincTableInterp2[i]) * alpha + Wavetables::sincTableInterp3[i]) * alpha + Wavetables::sincTable[i];
        break;
    case Staircase:
        value = ((Wavetables::staircaseTableInterp1[i] * alpha + Wavetables::staircaseTableInterp2[i]) * alpha + Wavetables::staircaseTableInterp3[i]) * alpha + Wavetables::staircaseTable[i];
        break;
    case SinStaircase:
        value = ((Wavetables::sinStaircaseTableInterp1[i] * alpha + Wavetables::sinStaircaseTableInterp2[i]) * alpha + Wavetables::sinStaircaseTableInterp3[i]) * alpha + Wavetables::sinStaircaseTable[i];
        break;
    case Hann:
        value = ((Wavetables::hannTableInterp1[i] * alpha + Wavetables::hannTableInterp2[i]) * alpha + Wavetables::hannTableInterp3[i]) * alpha + Wavetables::hannTable[i];
        break;
    case Gauss:
        value = ((Wavetables::gaussTableInterp1[i] * alpha + Wavetables::gaussTableInterp2[i]) * alpha + Wavetables::gaussTableInterp3[i]) * alpha + Wavetables::gaussTable[i];
        break;
    case BandlimitedSignal:
        value = ((Wavetables::bandlimitedSignalTableInterp1[i] * alpha + Wavetables::bandlimitedSignalTableInterp2[i]) * alpha + Wavetables::bandlimitedSignalTableInterp3[i]) * alpha + Wavetables::bandlimitedSignalTable[i];
        break;
    case BandlimitedImpulseTrain:
        value = ((Wavetables::bandlimitedImpulseTableInterp1[i] * alpha + Wavetables::bandlimitedImpulseTableInterp2[i]) * alpha + Wavetables::bandlimitedImpulseTableInterp3[i]) * alpha + Wavetables::bandlimitedImpulseTable[i];
        break;
    case BandlimitedTriangle:
        value = ((Wavetables::bandlimitedTriangleTableInterp1[i] * alpha + Wavetables::bandlimitedTriangleTableInterp2[i]) * alpha + Wavetables::bandlimitedTriangleTableInterp3[i]) * alpha + Wavetables::bandlimitedTriangleTable[i];
        break;
    case BandlimitedSawtooth:
        value = ((Wavetables::bandlimitedSawtoothTableInterp1[i] * alpha + Wavetables::bandlimitedSawtoothTableInterp2[i]) * alpha + Wavetables::bandlimitedSawtoothTableInterp3[i]) * alpha + Wavetables::bandlimitedSawtoothTable[i];
        break;
    case BandlimitedReverseSawtooth:
        value = ((Wavetables::bandlimitedReverseSawtoothTableInterp1[i] * alpha + Wavetables::bandlimitedReverseSawtoothTableInterp2[i]) * alpha + Wavetables::bandlimitedReverseSawtoothTableInterp3[i]) * alpha + Wavetables::bandlimitedReverseSawtoothTable[i];
        break;
    case BandlimitedSquare:
        value = ((Wavetables::bandlimitedSquareTableInterp1[i] * alpha + Wavetables::bandlimitedSquareTableInterp2[i]) * alpha + Wavetables::bandlimitedSquareTableInterp3[i]) * alpha + Wavetables::bandlimitedSquareTable[i];
        break;
             */
    default:
        value = ((Wavetables::sinTableInterp1[i] * alpha + Wavetables::sinTableInterp2[i]) * alpha + Wavetables::sinTableInterp3[i]) * alpha + Wavetables::sinTable[i];
        break;
    }
    
    return value;
}


float WavetableOscillator::updateBipolarValueWithCubicInterpolation()
{
    
    tableSize = Wavetables::sinTableBipolar.size(); //all tables must be the same size
    phase = fmod((phase + increment), tableSize);
    
    int i = static_cast<int>(phase);
    
    if (i < 0)
    {
        i = 0;
    }
    
    float alpha = phase - static_cast<float>(i);
    
    
    /* //remember to wrap around!!!
     dtable1[i] = (3.f*(table[i]-table[i+1])-table[i-1]+table[i+2])/2.f
     dtable2[i] = 2.f*table[i+1]+table[i-1]-(5.f*table[i]+table[i+2])/2.f
     dtable3[i] = (table[i+1]-table[i-1])/2.f
     */
    
    float value;
    switch (shape)
    {
        case Sinusoidal:
            value = ((Wavetables::sinTableInterp1Bipolar[i] * alpha + Wavetables::sinTableInterp2Bipolar[i]) * alpha + Wavetables::sinTableInterp3Bipolar[i]) * alpha + Wavetables::sinTableBipolar[i];
            break;
        case Triangle:
            value =  ((Wavetables::triangleTableInterp1Bipolar[i] * alpha + Wavetables::triangleTableInterp2Bipolar[i]) * alpha + Wavetables::triangleTableInterp3Bipolar[i]) * alpha + Wavetables::triangleTableBipolar[i];
            break;
        case Sawtooth:
            value = ((Wavetables::sawtoothTableInterp1Bipolar[i] * alpha + Wavetables::sawtoothTableInterp2Bipolar[i]) * alpha + Wavetables::sawtoothTableInterp3Bipolar[i]) * alpha + Wavetables::sawtoothTableBipolar[i];
            break;
        case ReverseSawtooth:
            value = ((Wavetables::reverseSawtoothTableInterp1Bipolar[i] * alpha + Wavetables::reverseSawtoothTableInterp2Bipolar[i]) * alpha + Wavetables::reverseSawtoothTableInterp3Bipolar[i]) * alpha + Wavetables::reverseSawtoothTableBipolar[i];
            break;
        case Square:
            value = ((Wavetables::squareTableInterp1Bipolar[i] * alpha + Wavetables::squareTableInterp2Bipolar[i]) * alpha + Wavetables::squareTableInterp3Bipolar[i]) * alpha + Wavetables::squareTableBipolar[i];
            break;
        default:
            value = ((Wavetables::sinTableInterp1Bipolar[i] * alpha + Wavetables::sinTableInterp2Bipolar[i]) * alpha + Wavetables::sinTableInterp3Bipolar[i]) * alpha + Wavetables::sinTableBipolar[i];
            break;
    }
    
    return value;
}

