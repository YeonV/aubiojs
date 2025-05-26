#include "aubio.h"
#include <emscripten/bind.h>
#include <string> // Added for std::string

using namespace emscripten;

class Tempo {
public:
  Tempo(std::string method, uint_t buf_size, uint_t hop_size, uint_t sample_rate) {
    buffer = new_fvec(hop_size);
    // Ensure output is initialized before potential use in methods called before _do
    output = new_fvec(1);
    aubio_tempo = new_aubio_tempo(method.c_str(), buf_size, hop_size, sample_rate);
  }

  ~Tempo() {
    del_aubio_tempo(aubio_tempo);
    del_fvec(buffer);
    del_fvec(output);
  }

  float _do(val input) {
    for (int i = 0; i < buffer->length; i += 1) {
      buffer->data[i] = input[i].as<float>();
    }
    aubio_tempo_do(aubio_tempo, buffer, output);
    return output->data[0];
  }

  float getConfidence() { return aubio_tempo_get_confidence(aubio_tempo); }

  float getBpm() { return aubio_tempo_get_bpm(aubio_tempo); }

  // New methods based on plan
  smpl_t getLast_s() { return aubio_tempo_get_last_s(aubio_tempo); }

  uint_t setSilence(smpl_t silence) { return aubio_tempo_set_silence(aubio_tempo, silence); }
  smpl_t getSilence() { return aubio_tempo_get_silence(aubio_tempo); }

  uint_t setThreshold(smpl_t threshold) { return aubio_tempo_set_threshold(aubio_tempo, threshold); }
  smpl_t getThreshold() { return aubio_tempo_get_threshold(aubio_tempo); }

  uint_t setDelay_s(smpl_t delay) { return aubio_tempo_set_delay_s(aubio_tempo, delay); }
  smpl_t getDelay_s() { return aubio_tempo_get_delay_s(aubio_tempo); }

private:
  aubio_tempo_t *aubio_tempo;
  fvec_t *buffer;
  fvec_t *output; // Initialized in constructor
};

EMSCRIPTEN_BINDINGS(Tempo) {
  class_<Tempo>("Tempo")
      .constructor<std::string, uint_t, uint_t, uint_t>() // Updated constructor
      .function("do", &Tempo::_do)
      .function("getBpm", &Tempo::getBpm)
      .function("getConfidence", &Tempo::getConfidence)
      .function("getLast_s", &Tempo::getLast_s)
      .function("setSilence", &Tempo::setSilence)
      .function("getSilence", &Tempo::getSilence)
      .function("setThreshold", &Tempo::setThreshold)
      .function("getThreshold", &Tempo::getThreshold)
      .function("setDelay_s", &Tempo::setDelay_s)
      .function("getDelay_s", &Tempo::getDelay_s);
}