#include "TSite.h"

TSite::TSite() {
  x = 0;
  y = 0;
}

TSite::TSite(int X, int Y) {
  x = X;
  y = Y;
}

TSite::~TSite() {
  //dtor
}

void TSite::Translate(int dx, int dy) {

  x = (x + dx + Lx) % Lx;
  y = (y + dy + Ly) % Ly;

}

std::ostream &operator<<(std::ostream &os, const TSite &me) {
  return os <<  me.x << "\t" << me.y;
}

bool operator==(const TSite &a, const TSite &b) {
  if ((a.x == b.x) && (a.y == b.y)) return true;
  else return false;
}
