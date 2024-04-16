/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef AMP_MODELER_COMMON
#define AMP_MODELER_COMMON

#include "juce_core/juce_core.h"
#include "juce_audio_processors/juce_audio_processors.h"

typedef float sample_t;

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

#define DB_TO_GAIN(value) (std::pow(10, value/20.0))
#define GAIN_TO_DB(value) (20.0 * std::log10(value))

#define CLIP(x, min, max) (x > max ? max : x < min ? min : x)

static inline float scale(float x, float min, float max, float newmin, float newmax, float curve) {
    return std::pow((x - min) / (max - min), curve) * (newmax - newmin) + newmin;
}

static inline u64 nextPowTwo(u64 n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}


#endif  // AMP_MODELER_COMMON
