/*
  ==============================================================================

    common.h
    Created: 23 Apr 2023 11:32:09am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef AMP_MODELER_COMMON
#define AMP_MODELER_COMMON

#include <JuceHeader.h>

typedef float sample_t;
using AudioBlock = juce::dsp::AudioBlock<sample_t>;

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

#define DB_TO_GAIN(value) (pow(10, value /20.0))
#define GAIN_TO_DB(value) (20.0 * log10(value))

#define CLIP(x, min, max) (x > max ? max : x < min ? min : x)

static inline float scale(float x, float min, float max, float newmin, float newmax, float curve) {
    return std::powf((x - min) / (max - min), curve) * (newmax - newmin) + newmin;
}


#endif  // AMP_MODELER_COMMON
