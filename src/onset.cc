#include "aubio.h"
#include <emscripten/bind.h>
#include <string> // Added for std::string

using namespace emscripten;

class Onset {
public:
  Onset(std::string method, uint_t buf_size, uint_t hop_size, uint_t sample_rate) {
    buffer = new_fvec(hop_size);
    // Ensure output is initialized before potential use in methods called before _do
    output = new_fvec(1);
    aubio_onset = new_aubio_onset(method.c_str(), buf_size, hop_size, sample_rate);
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

  // Existing method
  float getCompression() { return aubio_onset_get_compression(aubio_onset); }
  // New methods based on plan
  uint_t setCompression(smpl_t lambda) { return aubio_onset_set_compression(aubio_onset, lambda); }

  smpl_t getAwhitening() { return aubio_onset_get_awhitening(aubio_onset); }
  uint_t setAwhitening(uint_t enable) { return aubio_onset_set_awhitening(aubio_onset, enable); }

  smpl_t getLast_s() { return aubio_onset_get_last_s(aubio_onset); }

  uint_t setSilence(smpl_t silence) { return aubio_onset_set_silence(aubio_onset, silence); }
  smpl_t getSilence() { return aubio_onset_get_silence(aubio_onset); }

  uint_t setThreshold(smpl_t threshold) { return aubio_onset_set_threshold(aubio_onset, threshold); }
  smpl_t getThreshold() { return aubio_onset_get_threshold(aubio_onset); }

  uint_t setMinioi_s(smpl_t minioi) { return aubio_onset_set_minioi_s(aubio_onset, minioi); }
  smpl_t getMinioi_s() { return aubio_onset_get_minioi_s(aubio_onset); }

  uint_t setDelay_s(smpl_t delay) { return aubio_onset_set_delay_s(aubio_onset, delay); }
  smpl_t getDelay_s() { return aubio_onset_get_delay_s(aubio_onset); }

private:
  aubio_onset_t *aubio_onset;
  fvec_t *buffer;
  fvec_t *output; // Initialized in constructor
};

EMSCRIPTEN_BINDINGS(Onset) {
  class_<Onset>("Onset")
      .constructor<std::string, uint_t, uint_t, uint_t>() // Updated constructor
      .function("do", &Onset::_do)
      .function("getCompression", &Onset::getCompression)
      .function("setCompression", &Onset::setCompression)
      .function("getAwhitening", &Onset::getAwhitening)
      .function("setAwhitening", &Onset::setAwhitening)
      .function("getLast_s", &Onset::getLast_s)
      .function("setSilence", &Onset::setSilence)
      .function("getSilence", &Onset::getSilence)
      .function("setThreshold", &Onset::setThreshold)
      .function("getThreshold", &Onset::getThreshold)
      .function("setMinioi_s", &Onset::setMinioi_s)
      .function("getMinioi_s", &Onset::getMinioi_s)
      .function("setDelay_s", &Onset::setDelay_s)
      .function("getDelay_s", &Onset::getDelay_s);
}