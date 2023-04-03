/*
 * Architektury výpočetních systémů (AVS 2019)
 * Projekt c. 1 (ANN)
 * Login: xgrofc00
 */

#include <cstdlib>
#include "neuron.h"

float evalNeuron(
  size_t inputSize,
  //size_t neuronCount,
  const float* input,
  const float* weights,
  float bias
  //size_t neuronId
)
{
  //TODO: Step0 - Fill in the implementation, all the required arguments are passed.
  //              If you don't use them all you are doing something wrong!
  float k=0;
  #pragma omp simd aligned(input,weights)
  for (size_t i = 0; i < inputSize; i++) {
    k+=input[i]*weights[i];
  }
  k+=bias;
  return 0.0 > k ? 0.0 : k;
}
