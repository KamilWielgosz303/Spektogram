#ifndef WAV_PARSER_H
#define WAV_PARSER_H

#include <QObject>


struct  WAV_HEADER{
    char                RIFF[4];        // RIFF Header      Magic header
    unsigned long       ChunkSize;      // RIFF Chunk Size
    char                WAVE[4];        // WAVE Header
    char                fmt[4];         // FMT header
    unsigned long       Subchunk1Size;  // Size of the fmt chunk
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz
    unsigned long       bytesPerSec;    // bytes per second
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    unsigned short      bitsPerSample;  // Number of bits per sample
    char                Subchunk2ID[4]; // "data"  string
    unsigned long       Subchunk2Size;  // Sampled data length

};

//struct chunk
//{
//    char        id[4];
//    quint32     size;
//};

//struct RIFFHeader
//{
//    chunk       descriptor;     // "RIFF"
//    char        type[4];        // "WAVE"
//};

//struct WAVEHeader
//{
//    chunk       descriptor;
//    quint16     audioFormat;
//    quint16     numChannels;
//    quint32     sampleRate;
//    quint32     byteRate;
//    quint16     blockAlign;
//    quint16     bitsPerSample;
//    char        dataString[4];
//    quint32     dataSize;

//};

//struct DATAHeader
//{
//    chunk       descriptor;
//};

//struct CombinedHeader
//{
//    RIFFHeader  riff;
//    WAVEHeader  wave;
//};

#endif // WAV_PARSER_H
