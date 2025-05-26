// Common input buffer type
type InputBuffer = Float32Array | number[];

// Definition for AubioFVec (represents fvec_t*)
/**
 * Represents an fvec_t structure from Aubio, typically a pointer in Emscripten's memory.
 */
interface AubioFVec {
  /** The number of elements in the vector. */
  readonly length: number;
  /** 
   * A pointer (memory address) to the actual Float32Array data in Emscripten's HEAP.
   * Use Module.HEAPF32.subarray(data_ptr / 4, data_ptr / 4 + length) to access it.
   */
  readonly data_ptr: number; 
}

// Pitch Method and Unit Types
/**
 * Pitch detection methods available in Aubio.
 */
declare type PitchMethod =
  | "default"
  | "yin"
  | "mcomb"
  | "schmitt"
  | "fcomb"
  | "yinfft"
  | "yinfast"
  | "specacf";

declare type PitchUnit = "Hz" | "midi" | "cent" | "bin";

// Onset Method Type
declare type OnsetMethod =
  | "default"
  | "energy"
  | "hfc"
  | "complex"
  | "phase"
  | "specdiff"
  | "kl"
  | "mkl"
  | "specflux";

// Tempo Method Type
declare type TempoMethod = "default" | string;

// Class Definitions
declare class Pitch {
  constructor(method: PitchMethod, bufferSize: number, hopSize: number, sampleRate: number);
  do(buffer: InputBuffer): number;
  getTolerance(): number;
  setTolerance(tolerance: number): number;
  getSilence(): number;
  setSilence(silence: number): number;
  getConfidence(): number;
  setUnit(unit: PitchUnit): number;
  delete(): void;
}

declare class Tempo {
  constructor(method: TempoMethod, bufferSize: number, hopSize: number, sampleRate: number);
  do(buffer: InputBuffer): number;
  getBpm(): number;
  getConfidence(): number;
  getLast_s(): number;
  setSilence(silence: number): number;
  getSilence(): number;
  setThreshold(threshold: number): number;
  getThreshold(): number;
  setDelay_s(delay: number): number;
  getDelay_s(): number;
  delete(): void;
}

declare class Onset {
  constructor(method: OnsetMethod, bufferSize: number, hopSize: number, sampleRate: number);
  do(buffer: InputBuffer): number;
  getCompression(): number;
  setCompression(lambda: number): number;
  getAwhitening(): number;
  setAwhitening(enable: number): number;
  getLast_s(): number;
  setSilence(silence: number): number;
  getSilence(): number;
  setThreshold(threshold: number): number;
  getThreshold(): number;
  setMinioi_s(minioi: number): number;
  getMinioi_s(): number;
  setDelay_s(delay: number): number;
  getDelay_s(): number;
  delete(): void;
}

/**
 * Class for performing Fast Fourier Transform (FFT) using Aubio.
 */
declare class AubioFFT {
  /**
   * Creates a new AubioFFT instance.
   * @param bufferSize The size of the FFT buffer (e.g., 512, 1024). This should typically be a power of 2.
   */
  constructor(bufferSize: number);

  /**
   * Performs the FFT on the input audio data.
   * The results (magnitudes and phases) can be retrieved using get_magnitudes() and get_phases() after calling this.
   * @param inputBuffer A Float32Array containing the time-domain audio samples. Its length must match the bufferSize.
   */
  do_fft(inputBuffer: Float32Array): void;

  /**
   * Retrieves the magnitude spectrum from the last FFT operation.
   * @returns An AubioFVec object representing the magnitude data.
   */
  get_magnitudes(): AubioFVec;

  /**
   * Retrieves the phase spectrum from the last FFT operation.
   * @returns An AubioFVec object representing the phase data.
   */
  get_phases(): AubioFVec;

  /**
   * Releases the underlying C++ object's memory.
   */
  delete(): void;
}

// Main Aubio Module Type
declare type Aubio = {
  Pitch: {
    new (
      method: PitchMethod,
      bufferSize: number,
      hopSize: number,
      sampleRate: number
    ): Pitch;
  };
  Tempo: {
    new (
      method: TempoMethod, // Updated constructor
      bufferSize: number,
      hopSize: number,
      sampleRate: number
    ): Tempo;
  };
  Onset: {
    new (
      method: OnsetMethod, // Updated constructor
      bufferSize: number,
      hopSize: number,
      sampleRate: number
    ): Onset;
  };
  AubioFFT: {
    new (bufferSize: number): AubioFFT; // Added AubioFFT
  };
};

// Default export
/**
 * aubio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
declare function aubio(): Promise<Aubio>;

export default aubio;