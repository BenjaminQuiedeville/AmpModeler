#include <iostream>
#include <string>
#include <cstdint>
#include <assert.h>
#include <memory>

#define BITS_24_MAX (double)(1 << 23 - 1)

struct Signal {

    float *data = nullptr;
    size_t size = 0;
    double samplerate = 0.0;

    ~Signal() {
        if (data != nullptr) {
            free(data);
        }
    }
};

struct ChunkData {

    char riffString[4];
    int32_t fileSize; 
    char format[4];

    char subChunk1ID[4];
    int32_t subChunk1Size;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t samplerate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
    
    char subChunk2ID[4];
    int32_t signalSizeBytes;
};

void wavParse(FILE *wavFile, Signal *outSignal) {

    ChunkData chunk_data;

    int8_t temp[1] = {0};
    int8_t buffer[4] = {0};
    
    fread(chunk_data.riffString, 1, 4, wavFile);
    fread((int8_t *)(&chunk_data.fileSize), 1, 4, wavFile);
    
    int8_t *file_data = (int8_t *)malloc(chunk_data.fileSize);
    fread(file_data, 1, chunk_data.fileSize, wavFile);
    int file_data_index = 0;
        
    memcpy(chunk_data.format, file_data + file_data_index, 4);
    file_data_index += 4;
    
    assert(memcmp(chunk_data.format, "WAVE", 4) == 0);
    
    printf("RIFF String = %4s \n", chunk_data.riffString);
    printf("fileSize = %d \n", chunk_data.fileSize);
    printf("format = %4s \n", chunk_data.format);

    while (memcmp(file_data + file_data_index, "fmt ", 4)) {
        file_data_index++;
    }
    
    memcpy(chunk_data.subChunk1ID, file_data + file_data_index, 4);
    file_data_index += 4;

    assert(memcmp(chunk_data.subChunk1ID, "fmt ", 4) == 0);

    chunk_data.subChunk1Size = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.subChunk1Size);
    
    chunk_data.audioFormat = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.audioFormat);
    
    chunk_data.numChannels = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.numChannels);
    
    chunk_data.samplerate = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.samplerate);
    
    chunk_data.byteRate = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.byteRate);
    
    chunk_data.blockAlign = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.blockAlign);
    
    chunk_data.bitsPerSample = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.bitsPerSample);
        
    assert(chunk_data.numChannels == 1);
    printf("subChunk1ID = %4s \n", chunk_data.subChunk1ID);
    printf("subChunk1Size = %d \n", chunk_data.subChunk1Size);
    printf("audioFormat = %d \n", chunk_data.audioFormat);

    printf("numChannels = %d \n", chunk_data.numChannels);
    printf("samplerate = %d \n", chunk_data.samplerate);
    printf("byteRate = %d \n", chunk_data.byteRate);
    printf("blockAlign = %d \n", chunk_data.blockAlign);
    printf("bitsPerSample = %d \n", chunk_data.bitsPerSample);

    if (chunk_data.byteRate != (chunk_data.samplerate * chunk_data.numChannels * chunk_data.bitsPerSample/8)) {
        printf("byte Rate error");
    }

    if (chunk_data.blockAlign != (chunk_data.numChannels * chunk_data.bitsPerSample/8)) {
        printf("blockAlign Rate error");
    }

    while (memcmp(file_data + file_data_index, "data", 4)) {
        file_data_index++;
    }
    
    memcpy(chunk_data.subChunk2ID, file_data + file_data_index, 4);
    file_data_index += 4;
    
    assert(memcmp(chunk_data.subChunk2ID, "data", 4) == 0);

    printf("subChunk2ID = %4s \n", chunk_data.subChunk2ID);

    chunk_data.signalSizeBytes = *(int32_t*)(file_data + file_data_index);
    file_data_index += sizeof(chunk_data.signalSizeBytes);
    
    printf("signalSizeBytes = %d \n", chunk_data.signalSizeBytes);
    
    size_t sampleSizeBytes = chunk_data.blockAlign/chunk_data.numChannels;
    size_t numSamples = chunk_data.signalSizeBytes/sampleSizeBytes;


    outSignal->samplerate = (double)chunk_data.samplerate;
    outSignal->size = numSamples;
    outSignal->data = (float *)calloc(numSamples, sizeof(float));
    
    int8_t *signal_ptr = file_data + file_data_index;
    
    uint32_t byte_counter = 0;
    uint32_t sampleCounter = 0;
    
    if (chunk_data.bitsPerSample == 16) {

        int8_t sampleChar[2] = {0x00, 0x00};
        
        while (sampleCounter < numSamples && byte_counter < chunk_data.signalSizeBytes) {
            
            memcpy(sampleChar, signal_ptr + byte_counter, 2);
            byte_counter += 2;
                        
            int32_t mask = 0x01 << (chunk_data.bitsPerSample - 1);
            int16_t sampleInt = *(int16_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX / 2.0;

            outSignal->data[sampleCounter] = sample;
            sampleCounter++;
        }

    } else if (chunk_data.bitsPerSample == 24) {
    
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (sampleCounter < numSamples && byte_counter < chunk_data.signalSizeBytes) {
            
            memcpy(sampleChar, signal_ptr + byte_counter, 3);
            byte_counter += 3;

            int64_t mask = 0x01 << (chunk_data.bitsPerSample - 1);
            // uint32_t mask = 0x800000;
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            float sample = sampleInt / BITS_24_MAX / 2.0;

            outSignal->data[sampleCounter] = sample;
            sampleCounter++;
        }

    } else if (chunk_data.bitsPerSample == 32) {
        
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};

        while (sampleCounter < numSamples && byte_counter < chunk_data.signalSizeBytes) {

            memcpy(sampleChar, signal_ptr + byte_counter, 4);
            byte_counter += 4;
            
            int64_t mask = 0x01 << (chunk_data.bitsPerSample - 1);
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFFFF;
            }

            float sample = sampleInt / (float)_I32_MAX / 2.0;

            outSignal->data[sampleCounter] = sample;
            sampleCounter++;
        }
    }

    printf("\n");    

    free(file_data);

    return;
}

int main(int argc, char **argv) {

    // std::string filepath = "../data/default_IR_48.wav";
    std::string filepath = "D:/Projets musique/ImpulseResponses/Aurora/44.1k/AURORA_212_CAD_CAP.wav";

    std::cout << "filepath : " << filepath << "\n";
    
    Signal signal;

    FILE *wavFile = fopen(filepath.data(), "rb");

    wavParse(wavFile, &signal);

    for (size_t i = 0; i < 50; i++) {
        printf("%.3f ", signal.data[i]);
    }

    fclose(wavFile);

    return 0;
}