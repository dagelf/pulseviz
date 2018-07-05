#define _USE_MATH_DEFINES
#include <math.h>
#include "stft.h"
#include "util.h"

STFT::STFT(SimpleRecordClient& src, size_t sample_size, size_t window_size, float window_overlap, Window window)
    :
    coefficients(sample_size / 2 + 1),
    sample_size(sample_size),
    frequencies(sample_size / 2 + 1),
    sampler(
        src,
        sample_size,
        std::floor((float)window_size * (1.0 - window_overlap))
    )
{
    this->in = fftwf_alloc_real(sample_size);
    this->out = fftwf_alloc_complex(this->coefficients.size());
    this->plan = fftwf_plan_dft_r2c_1d(sample_size, this->in, this->out, FFTW_ESTIMATE);

    switch (window)
    {
        case STFT::Window::RECTANGLE: this->window = this->generateRectangleWindow(sample_size); break;
        case STFT::Window::HAMMING: this->window = this->generateHammingWindow(sample_size); break;
    }

    this->window_sum = 0.0;
    for (float& value: this->window)
        this->window_sum += value;

    for (unsigned int i = 0; i < this->coefficients.size(); i++)
        this->frequencies[i] = static_cast<float>(i) / static_cast<float>(this->coefficients.size() - 1) * src.getSampleRate() / 2.0f;
}

STFT::~STFT()
{
    fftwf_destroy_plan(plan);
    fftwf_free(this->in);
    fftwf_free(this->out);
}

void STFT::slide()
{
    this->sampler.readChunk();
    this->sampler.appendToBuffer();

    for (unsigned int i = 0; i < this->sampler.data.size(); i++)
        this->in[i] = this->sampler.data[i] * this->window[i];

    fftwf_execute(plan);

    for (size_t i = 0; i < this->coefficients.size(); i++)
    {
        float magnitude = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);

        // "Whenever doing windowing, it is necessary to compensate for
        // loss of energy due to multiplication by that window.
        // This is defined as division by sum of window samples (sum(win)).
        // In case of rectangular window (or now window), it is as simple as
        // division by N, where N is the DFT length." - jojek
        // Source: https://dsp.stackexchange.com/a/32080
        magnitude *= 2.0;
        magnitude /= this->window_sum;

        this->coefficients[i] = magnitude;
    }
}

const std::vector<float>& STFT::getFrequencies()
{
    return this->frequencies;
}

float STFT::getBinWidth()
{
    return (2.0f / (float)this->sample_size) * (this->sampler.src.getSampleRate() / 2.0f);
}

std::vector<float> STFT::generateHammingWindow(unsigned int n)
{
    std::vector<float> result(n);
    for (unsigned int i = 0; i < n; i++)
        result[i] = 25.0/46.0 - 21.0/46.0 * cos(2 * M_PI * i / (n - 1));
    return result;
}

float STFT::convertToDecibel(float magnitude)
{
    return ::convertToDecibel(magnitude, STFT::min_dB);
}

std::vector<float> STFT::generateRectangleWindow(unsigned int n)
{
    std::vector<float> result(n);
    for (unsigned int i = 0; i < n; i++)
        result[i] = 1.0;
    return result;
}
