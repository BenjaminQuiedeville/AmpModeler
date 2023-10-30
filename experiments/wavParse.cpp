#include <iostream>
#include <fstream>
#include <cstdint>
#include <assert.h>
#include <memory.h>

#define BITS_24_MAX (double)(1 << 23 - 1)

struct Signal {

    // mono signal;

    float *data;
    size_t size;
    double samplerate;

    Signal() {
        data = nullptr;
        size = 0;
        samplerate = 0.0;
    }

    ~Signal() {
        if (data != nullptr) {
            free(data);
        }
    }
};

void wavParse(FILE *wavFile, Signal *outSignal) {

    char temp[1] = {0};
    char riffString[4];
    int32_t ChunkSize; 
    char format[4];

    char SubChunk1ID[4];
    int32_t SubChunk1Size;
    int16_t AudioFormat;
    int16_t NumChannels;
    int32_t samplerate;
    int32_t byteRate;
    int16_t BlockAlign;
    int16_t bitsPerSample;
    
    char SubChunk2ID[4];
    int32_t signalSizeBytes;
    
    fread(riffString, 1, 4, wavFile);
    fread((char *)(&ChunkSize), 1, 4, wavFile);
    fread(format, 1, 4, wavFile);
    
    printf("%4s \n", riffString);
    printf("ChunkSize = %d \n", ChunkSize);
    printf("format = %4s \n", format);

    while (temp[0] != 'f') {
        fread(temp, 1, 1, wavFile);
    }

    SubChunk1ID[0] = temp[0];
    temp[0] = 0;

    fread(SubChunk1ID+1, 1, 3, wavFile);

    fread((char *)(&SubChunk1Size), 1, 4, wavFile);
    fread((char *)(&AudioFormat), 1, 2, wavFile);

    fread((char *)(&NumChannels), 1, 2, wavFile);   
    fread((char *)(&samplerate), 1, 4, wavFile);    
    fread((char *)(&byteRate), 1, 4, wavFile);  
    fread((char *)(&BlockAlign), 1, 2, wavFile);    
    fread((char *)(&bitsPerSample), 1, 2, wavFile);
    
    assert(NumChannels == 1);
    printf("SubChunk1ID = %4s \n", SubChunk1ID);
    printf("SubChunk1Size = %d \n", SubChunk1Size);
    printf("AudioFormat = %d \n", AudioFormat);

    printf("NumChannels = %d \n", NumChannels);
    printf("samplerate = %d \n", samplerate);
    printf("byteRate = %d \n", byteRate);
    printf("BlockAlign = %d \n", BlockAlign);
    printf("bitsPerSample = %d \n", bitsPerSample);

    if (byteRate != (samplerate * NumChannels * bitsPerSample/8)) {
        printf("byte Rate errone");
    }

    if (BlockAlign != (NumChannels * bitsPerSample/8)) {
        printf("BlockAlign Rate errone");
    }

    while (temp[0] != 'd') {
        fread(temp, 1, 1, wavFile);
    }

    SubChunk2ID[0] = temp[0];
    temp[0] = 0;
    fread(SubChunk2ID+1, 1, 3, wavFile);
    

    if (SubChunk2ID[0] != 'd' || SubChunk2ID[1] != 'a' 
     || SubChunk2ID[2] != 't' || SubChunk2ID[3] != 'a') 
    {
        while (temp[0] != 'd') {
            fread(temp, 1, 1, wavFile);
        }

        SubChunk2ID[0] = temp[0];
        fread(SubChunk2ID+1, 1, 3, wavFile);

    }

    assert((SubChunk2ID[0] == 'd' || SubChunk2ID[1] == 'a' 
         || SubChunk2ID[2] == 't' || SubChunk2ID[3] == 'a'));

    printf("SubChunk2ID = %4s \n", SubChunk2ID);

    fread(&signalSizeBytes, 4, 1, wavFile);
    printf("signalSizeBytes = %d \n", signalSizeBytes);
    
    size_t sampleSizeBytes = BlockAlign/NumChannels;
    size_t numSamples = signalSizeBytes/sampleSizeBytes;


    outSignal->samplerate = (double)samplerate;
    outSignal->size = numSamples;
    outSignal->data = (float *)calloc(numSamples, sizeof(float));

    uint32_t counter = 0;
    if (bitsPerSample == 16) {

        int8_t sampleChar[2] = {0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 2, wavFile);
            
            int32_t mask = 0x01 << (bitsPerSample - 1);
            int16_t sampleInt = *(int16_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX;

            outSignal->data[counter] = sample;
            counter++;
        }

    } else if (bitsPerSample == 24) {
    
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 3, wavFile);

            int64_t mask = 0x01 << (bitsPerSample - 1);
            // uint32_t mask = 0x800000;
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            float sample = sampleInt / BITS_24_MAX / 2.0;

            outSignal->data[counter] = sample;
            counter++;
        }

    } else if (bitsPerSample == 32) {
        
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {

            fread(sampleChar, 1, 4, wavFile);

            int64_t mask = 0x01 << (bitsPerSample - 1);
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFFFF;
            }

            float sample = sampleInt / (float)_I32_MAX;

            outSignal->data[counter] = sample;
            counter++;
        }
    }

    printf("\n");    
    return;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("erreur, pas de nom de fichier donné\n");
        return 1;
    }

    std::string filepath = std::string(argv[1]);

    std::cout << "Parsing the file : \"" << filepath << "\"\n";

    FILE *wavFile;
    Signal *signal = new Signal();

    wavFile = fopen(argv[1], "rb");

    wavParse(wavFile, signal);

    for (size_t i = 0; i < 50; i++) {
        printf("%.3f ", signal->data[i]);
    }

    fclose(wavFile);

    delete signal;

    return 0;
}