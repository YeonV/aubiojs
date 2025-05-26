#include "aubio.h" // Should transitively include fvec.h, cvec.h, spectral/fft.h
#include <emscripten/bind.h>
#include <vector>   // For vecFromJSArray
#include <stdexcept> // For throwing errors

// Ensure smpl_t is float for JS TypedArray compatibility
#if !defined(AUBIO_SMPL_FLOAT) && !defined(AUBIO_SMPL_DOUBLE)
#error "aubio smpl_t type must be defined (float or double)"
#elif defined(AUBIO_SMPL_DOUBLE)
#warning "aubio smpl_t is double, JS interaction typically uses Float32Array. Data conversion might have precision implications or require Float64Array."
#endif

using namespace emscripten;

// AubioFFT C++ Class Definition
class AubioFFT {
public:
    uint_t buf_size;         // FFT size
    aubio_fft_t *fft_instance; // Aubio's FFT object
    cvec_t *spectrum_output; // Aubio's complex output format

    fvec_t *input_buffer;   // Internal buffer to store input from JS
    fvec_t *magnitudes;     // To store and return magnitude spectrum
    fvec_t *phases;         // To store and return phase spectrum

    AubioFFT(uint_t buffer_size) : buf_size(buffer_size) {
        if (buffer_size == 0) {
            throw std::runtime_error("AubioFFT: buffer_size cannot be zero.");
        }
        // Check if buffer_size is a power of 2, often a requirement for FFTs
        // Aubio might handle non-power-of-2 sizes, but it's good practice.
        // For now, assume aubio handles it or user provides valid size.

        fft_instance = new_aubio_fft(buf_size);
        if (!fft_instance) {
            throw std::runtime_error("AubioFFT: Failed to create aubio_fft_t instance.");
        }

        spectrum_output = new_cvec(buf_size); // new_cvec expects FFT size
        if (!spectrum_output) {
            del_aubio_fft(fft_instance); // Clean up previously allocated fft_instance
            throw std::runtime_error("AubioFFT: Failed to create cvec_t spectrum_output.");
        }

        input_buffer = new_fvec(buf_size);
        if (!input_buffer) {
            del_cvec(spectrum_output);
            del_aubio_fft(fft_instance);
            throw std::runtime_error("AubioFFT: Failed to create fvec_t input_buffer.");
        }
        
        // The length of spectrum_output->norm and spectrum_output->phas will be (buf_size / 2) + 1
        uint_t spectrum_length = spectrum_output->length;
        magnitudes = new_fvec(spectrum_length);
        if (!magnitudes) {
            del_fvec(input_buffer);
            del_cvec(spectrum_output);
            del_aubio_fft(fft_instance);
            throw std::runtime_error("AubioFFT: Failed to create fvec_t for magnitudes.");
        }

        phases = new_fvec(spectrum_length);
        if (!phases) {
            del_fvec(magnitudes);
            del_fvec(input_buffer);
            del_cvec(spectrum_output);
            del_aubio_fft(fft_instance);
            throw std::runtime_error("AubioFFT: Failed to create fvec_t for phases.");
        }
    }

    ~AubioFFT() {
        del_aubio_fft(fft_instance);
        del_cvec(spectrum_output);
        del_fvec(input_buffer);
        del_fvec(magnitudes);
        del_fvec(phases);
    }

    void do_fft_js(emscripten::val js_input_array) {
        std::vector<float> temp_vec = emscripten::vecFromJSArray<float>(js_input_array);

        if (temp_vec.size() != buf_size) {
            // For production, might be better to throw an exception
            // that can be caught in JavaScript.
            // Example: throw std::runtime_error("Input array size does not match FFT buffer size.");
            // For now, just print to console and return, or handle as per project's error strategy.
            // Alternatively, could throw an exception here:
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Input array size (%zu) does not match FFT buffer size (%u).", temp_vec.size(), buf_size);
            throw std::runtime_error(error_msg);
        }

        for (uint_t i = 0; i < buf_size; ++i) {
            input_buffer->data[i] = static_cast<smpl_t>(temp_vec[i]);
        }
        
        aubio_fft_do(fft_instance, input_buffer, spectrum_output);

        for (uint_t i = 0; i < spectrum_output->length; ++i) {
            magnitudes->data[i] = spectrum_output->norm[i];
            phases->data[i] = spectrum_output->phas[i];
        }
    }

    fvec_t* get_magnitudes() {
        return magnitudes;
    }

    fvec_t* get_phases() {
        return phases;
    }
};

// Emscripten Bindings
EMSCRIPTEN_BINDINGS(aubio_fft_module) {
    // Bind fvec_t to make it usable as a return type and allow JS to access its fields.
    // This JS object will be a wrapper around the C++ fvec_t*.
    value_object<fvec_t>("AubioFVec")
        .field("length", &fvec_t::length)
        .function("get_data_ptr", optional_override([](const fvec_t& vec) {
            return reinterpret_cast<uintptr_t>(vec.data);
        }));

    class_<AubioFFT>("AubioFFT")
        .constructor<uint_t>()
        .function("do_fft", &AubioFFT::do_fft_js) 
        .function("get_magnitudes", &AubioFFT::get_magnitudes, allow_raw_pointers()) 
        .function("get_phases", &AubioFFT::get_phases, allow_raw_pointers());
}
