#include "RandUtils.h"
#include <random>
#include <optional>

std::mt19937 &GetEngine(std::optional<uint64_t> seed = {}) {
  static std::random_device R;
  static std::mt19937 E(R());  //Mersenne Twister random numbers
  if(seed.has_value()){
    E.seed(seed.value());
  }
  return E;
}

int randM(int M) {
  return std::uniform_int_distribution<int>(0, M - 1)(GetEngine());
}

double ranMT() {
  return std::uniform_real_distribution<double>(0, 1)(GetEngine());
}
