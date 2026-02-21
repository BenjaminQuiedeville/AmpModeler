/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef AMP_MODELER_COMMON
#define AMP_MODELER_COMMON

#include "cassert"

// #define _USE_MATH_DEFINES
#include <cmath>
#include <stdint.h>

#ifdef TRACY_ENABLE
    #include <tracy/Tracy.hpp>
#else 
    #define ZoneScoped
    #define FrameMark
    #define ZoneScopedN(text)
#endif

// #define noinline __attribute__ ((noinline))

#define local_const static const
#define global_const static const

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

#define min(x, y) (x) < (y) ? (x) : (y)
#define max(x, y) (x) > (y) ? (x) : (y)

inline float dbtoa(float value) {
    return std::pow(10.0f, value/20.0f);
}

inline float atodb(float value) {
    return 20.0f * std::log10(value);
}

#define CLIP(x, min, max) ((x) > (max) ? (max) : (x) < (min) ? (min) : (x))

#define copyFloat(dest, source, nsamples)      memcpy(dest, source, (nsamples) * sizeof(float))
#define memsetZeroFloat(ptr, nsamples)         memset(ptr, 0, (nsamples) * sizeof(float))

#define allocFloat(nelems) (float*)calloc((nelems), sizeof(float))

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

enum ChannelConfig : u8 {
    Mono, 
    FakeStereo, 
    Stereo
};



struct Slice {
    float *dataL = nullptr;
    float *dataR = nullptr;
    u32 size = 0;
};

static void reallocMonoBuffer(Slice *buffer, u32 size) {
    if (buffer->dataL) { free(buffer->dataL); }
    buffer->size = size;
    buffer->dataL = allocFloat(buffer->size);
    memsetZeroFloat(buffer->dataL, buffer->size);
}

static inline void applyGainLinear(Slice buffer,float gain) {
    ZoneScoped;
    for (u32 index = 0; index < buffer.size; index++) {
        buffer.dataL[index] *= gain;
    }
    if (buffer.dataR) {
        for (u32 index = 0; index < buffer.size; index++) {
            buffer.dataR[index] *= gain;
        }
    }
}


#endif  // AMP_MODELER_COMMON
