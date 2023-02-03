#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define BUFFER_SIZE 512

typedef struct {
    char chunkID[4];
    int chunkSize;
    char format[4];
    char subchunk1ID[4];
    int subchunk1Size;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
    char subchunk2ID[4];
    int subchunk2Size;
} WAVHeader;

void equalize(double* input, double* output, int n, int fc, int bw, double gain) {
    for (int i = 0; i < n; i++) {
        double w = 2 * PI * fc * i / n;
        double b = bw * w / (2 * PI);
        double h = pow(10, gain / 20) * (1 + b) / (1 + b - cos(w));
        output[i] = input[i] * h;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file.wav output_file.wav\n", argv[0]);
        return 1;
    }

    FILE* inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    FILE* outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL) {
        printf("Error opening output file\n");
        return 1;
    }

    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, inputFile);

    int fc = 1000;
    int bw = 100;
    double gain = 10;

    int dataSize = header.subchunk2Size;
    short buffer[BUFFER_SIZE];
    double sample[BUFFER_SIZE];
    double output[BUFFER_SIZE];
    int read = 0;

    fwrite(&header, sizeof(WAVHeader), 1, outputFile);
    {
        while (dataSize > 0) {
        read = fread(buffer, sizeof(short), BUFFER_SIZE, inputFile);
        for (int i = 0; i < read; i++) {
            sample[i] = (double) buffer[i] / 32767.0;
        }
        equalize(sample, output, read, fc, bw, gain);
        for (int i = 0; i < read; i++) {
            buffer[i] = (short) (output[i] * 32767.0);
        }
        fwrite(buffer, sizeof(short), read, outputFile);
        dataSize
