#include "RandUtils.h"
#include <random>

std::mt19937 &GetEngine() {
  static std::random_device R;
  static std::mt19937 E(R());  //Mersenne Twister random numbers
  return E;
}

int randM(int M) {
  return std::uniform_int_distribution<int>(0, M - 1)(GetEngine());
}

double ranMT() {
  return std::uniform_real_distribution<double>(0, 1)(GetEngine());
}
