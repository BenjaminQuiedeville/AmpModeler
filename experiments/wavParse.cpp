#include <iostream>
#include <fstream>
#include <cstdint>



int main(int argc, char **argv) {

    std::string filepath = std::string(argv[1]);

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
    int16_t BitsPerSample;

    char SubChunk2ID[4];
    int32_t SubChunk2Size;

    std::ifstream wavFile;
    wavFile.open(filepath);

    wavFile.read(riffString, 4);
    printf("%4s \n", riffString);

    wavFile.read((char *)(&ChunkSize), 4);
    printf("ChunkSize = %d \n", ChunkSize);

    wavFile.read(format, 4);
    printf("format = %4s \n", format);

    while (temp[0] != 'f') {
        wavFile.read(temp, 1);
    }

    SubChunk1ID[0] = temp[0];

    wavFile.read(SubChunk1ID+1, 3);
    printf("SubChunk1ID = %4s \n", SubChunk1ID);

    wavFile.read((char *)(&SubChunk1Size), 4);
    printf("SubChunk1Size = %d \n", SubChunk1Size);
    
    wavFile.read((char *)(&AudioFormat), 2);
    printf("AudioFormat = %d \n", AudioFormat);

    wavFile.read((char *)(&NumChannels), 2);
    printf("NumChannels = %d \n", NumChannels);

    wavFile.read((char *)(&samplerate), 4);
    printf("samplerate = %d \n", samplerate);

    wavFile.read((char *)(&byteRate), 4);
    printf("byteRate = %d \n", byteRate);

    wavFile.read((char *)(&BlockAlign), 2);
    printf("BlockAlign = %d \n", BlockAlign);

    wavFile.read((char *)(&BitsPerSample), 2);
    printf("BitsPerSample = %d \n", BitsPerSample);

    if (byteRate != (samplerate * NumChannels * BitsPerSample/8)) {
        printf("byte Rate errone");
    }

    if (BlockAlign != (NumChannels * BitsPerSample/8)) {
        printf("BlockAlign Rate errone");
    }

    while (temp[0] != 'd') {
        wavFile.read(temp, 1);
    }

    SubChunk2ID[0] = temp[0];
    temp[0] = 0;
    wavFile.read(SubChunk2ID+1, 3);
    if (SubChunk2ID[0] != 'd' || SubChunk2ID[1] != 'a' || SubChunk2ID[2] != 't' || SubChunk2ID[3] != 'a') {
        while (temp[0] != 'd') {
            wavFile.read(temp, 1);
        }
        SubChunk2ID[0] = temp[0];
        wavFile.read(SubChunk2ID+1, 3);
    }

    printf("SubChunk2ID = %4s \n", SubChunk2ID);

    
    wavFile.read((char *)(&SubChunk2Size), 4);
    printf("SubChunk2Size = %d \n", SubChunk2Size);


    char *signalChar = (char *)malloc(SubChunk2Size);

    wavFile.read(signalChar, SubChunk1Size);

    size_t sampleSizeBytes = BitsPerSample/8;

    wavFile.close();

    size_t signalLength = SubChunk2Size/sampleSizeBytes;
    int32_t *signal = (int32_t *)calloc(signalLength, sizeof(int32_t));

    char tempSample[4] = {0, 0, 0, 0};
    for (size_t i = 0; i < signalLength; i++) {
        
        int index = i*sampleSizeBytes;

        int32_t sample = (signalChar[index] << 16) | (signalChar[index + 1] << 8) | (signalChar[index + 2]);
        
        if (sample & 0x800000) {//  if the 24th bit is set, this is a negative number in 24-bit world
            sample = sample | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float
        }

        signal[i] = sample;



        printf("%d ", signalChar[i]);
    }

    printf("\n");

    printf("Bravo tout est bon !\n");

    free(signalChar);
    free(signal);

    return 0;
}