/*
  ==============================================================================

    common.h
    Created: 23 Apr 2023 11:32:09am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef AMP_MODELER_COMMON
#define AMP_MODELER_COMMON

#include "juce_core/juce_core.h"
#include "juce_audio_processors/juce_audio_processors.h"

typedef float sample_t;

#define DB_TO_GAIN(value) (std::pow(10, value/20.0))
#define GAIN_TO_DB(value) (20.0 * std::log10(value))

#define CLIP(x, min, max) (x > max ? max : x < min ? min : x)

static inline float scale(float x, float min, float max, float newmin, float newmax, float curve) {
    return std::pow((x - min) / (max - min), curve) * (newmax - newmin) + newmin;
}


#endif  // AMP_MODELER_COMMON
