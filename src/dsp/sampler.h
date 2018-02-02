#ifndef SAMPLER_H
#define SAMPLER_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <fftw3.h>
#include "../pulseaudio.h"

class Sampler
{
public:
    Sampler(std::shared_ptr<SimpleRecordClient> src, size_t data_size, size_t chunk_size);
    void readChunk();
    void appendToBuffer();

    std::vector<float> data;

protected:
    std::vector<float> chunk;
    std::shared_ptr<SimpleRecordClient> src;
};

#endif // SAMPLER_H
