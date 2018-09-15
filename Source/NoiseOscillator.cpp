#include "NoiseOscillator.h"

NoiseOscillator::NoiseOscillator() :
m_distribution(-0.5, 0.5)
{

}

float NoiseOscillator::getNextSample()
{
    float value = m_distribution(m_generator);

    return value;
}

void NoiseOscillator::setRange(float min, float max)
{
    m_distribution = std::uniform_real_distribution<float>(min, max);
}
