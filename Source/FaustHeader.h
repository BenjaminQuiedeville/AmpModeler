#include "common.h"

// dummy base dsp class
struct dsp {};

// dummy metadata class for faust dsp
struct Meta {
    void declare(const char*, const char*) {}
};


global_const u32 nFaustParams = 5;

enum class FaustParams : u8 {
    Mix,
    Gain,
    High,
    Low,
    Volume,
};

struct UI {
    
    void openVerticalBox(const char *) {}
    void closeBox() {}
    
    void addHorizontalSlider(const char* label, float *zone, float init, float min, float max, float step) {
        
        assert(paramIndex < nFaustParams && "Not enough param zones in Faust UI class"); 
        
        zones[paramIndex] = zone;
        *zone = init;
        paramIndex++;
    }

    float *zones[nFaustParams] = {};
    u32 paramIndex = 0;
};
