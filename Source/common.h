/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef AMP_MODELER_COMMON
#define AMP_MODELER_COMMON

#include "juce_core/juce_core.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "cassert"

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

typedef float Sample;

inline double dbtoa(double value) {
    return std::pow(10, value/20.0);
}

inline double atodb(double value) {
    return 20.0 * std::log10(value);
}

#define CLIP(x, min, max) (x > max ? max : x < min ? min : x)


static inline float scale(float x, float min, float max, float newmin, float newmax, float curve) {
    return std::pow((x - min) / (max - min), curve) * (newmax - newmin) + newmin;
}

static inline float scale_linear(float x, float min, float max, float newmin, float newmax) {
    return (x - min) / (max - min) * (newmax - newmin) + newmin;
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
