#include "aubio.h"
#include <emscripten/bind.h>

using namespace emscripten;

class Onset {
public:
  Onset(uint_t buf_size, uint_t hop_size, uint_t sample_rate) {
    buffer = new_fvec(hop_size);
    aubio_onset = new_aubio_onset("default", buf_size, hop_size, sample_rate);
  }

  ~Onset() {
    del_aubio_onset(aubio_onset);
    del_fvec(buffer);
    del_fvec(output);
  }

  float _do(val input) {
    for (int i = 0; i < buffer->length; i += 1) {
      buffer->data[i] = input[i].as<float>();
    }
    aubio_onset_do(aubio_onset, buffer, output);
    return output->data[0];
  }

  float getCompression() { return aubio_onset_get_compression(aubio_onset); }

  float getAwhitening() { return aubio_onset_get_awhitening()(aubio_onset); }
//   float getAwhitening() { return aubio_onset_aubio_spectral_whitening_do()(aubio_onset); }

private:
  aubio_onset_t *aubio_onset;
  fvec_t *buffer;
  fvec_t *output = new_fvec(1);
};

EMSCRIPTEN_BINDINGS(Onset) {
  class_<Onset>("Onset")
      .constructor<uint_t, uint_t, uint_t>()
      .function("do", &Onset::_do)
      .function("getCompression", &Onset::getCompression);
      .function("getAwhitening", &Onset::getAwhitening);
}
