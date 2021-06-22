#include "TParticle.h"
static constexpr int Lx = TSite::Lx, Ly = TSite::Ly;

TParticle::TParticle(int pIndex)
    : Index(pIndex),
      is_freeL(true),
      is_freeR(true),
      LinkedWith{-1, -1, -1, -1}/* se aggiungo :is_freeL(true) lo inizializza direttamente vero*/
{
  RandomizePosition();
  RandomizeOrientation();

  is_activeA = true;
  is_activeB = true;

  mob = MobState::FREE;
  /*LinkedWith[0] = -1;
  LinkedWith[1] = -1;
  LinkedWith[2] = -1;
  LinkedWith[3] = -1;*/
}

TParticle::TParticle(int pIndex, TSite pSite, int pSpin)
    : Index(pIndex), CSite(pSite), is_freeL(true), is_freeR(true), LinkedWith{-1, -1, -1, -1}, Spin(pSpin) {
  RecalcExtSites();
  is_activeA = false;
  is_activeB = false;
  mob = MobState::FREE;
}

TParticle::~TParticle() {
  //dtor
}

bool TParticle::Evolve() {

  bool JustJoined = false;

  switch (mob) {
    case MobState::FREE: {

        TryActivateAB();
        ClearParticlePosition();

        if(FreeMove()) {

            for (int i : Lattice->GetSiteIndexes(CSite)) {
                if (i != Index) {
                    if (CheckJoinWithCSite(Lattice->GetParticle(i))) {
                        CheckBorder();
                        JustJoined = true;
                    }
                }
            }

            for (int i : Lattice->GetSiteIndexes(RSite)) {
                if (i != Index) {
                    if (CheckJoinWithRSite(Lattice->GetParticle(i))) {
                        CheckBorder();
                        JustJoined = true;
                    }
                }
            }

            for (int i : Lattice->GetSiteIndexes(LSite)) {
                if (i != Index) {
                    if (CheckJoinWithLSite(Lattice->GetParticle(i))) {
                        CheckBorder();
                        JustJoined = true;
                    }
                }
            }
        }
        SetParticlePosition();
        break;
    }

    case MobState::LINKED: {
      if (LinkedWith[1] == -1 && LinkedWith[2] == -1) TryActivateAB();
      if(CheckClose()) CheckBorder();
      break;
    }

    case MobState::BLOCKED: {
    }
  }
  return JustJoined;
}

void TParticle::TryActivateAB() {
  if (ranMT() > ACT_TRESH) {
    is_activeA = true;
    is_activeB = true;
  }
}

bool TParticle::FreeMove() {

  // Transalte CSite in one of the 6 neighbours Sites chosen randomly. Before check if there is the aggregate
  if (ranMT() < TRANSL_RATE) {

    int tr = randM(6);
    //if (CheckAggregate(TSite(CSite.x + dx[tr],CSite.y + dy[tr]))) return false;
    //if (CheckAggregate(TSite(LSite.x + dx[tr],LSite.y + dy[tr]))) return false;
    //if (CheckAggregate(TSite(RSite.x + dx[tr],RSite.y + dy[tr]))) return false;

    CSite.Translate(dx[tr], dy[tr]);
    // and by consequence LSite and RSite by the same dx and dy
    LSite.Translate(dx[tr], dy[tr]);
    RSite.Translate(dx[tr], dy[tr]);
  }

  // Rotate randomly clockwise or anticlockwise
  if (ranMT() < ZY_ROT_RATE) {
    int r = randM(2);
    Spin = (Spin + R[r] + 6) % 6;
    RecalcExtSites();
  }

  //Rotate on his own axis
  if (ranMT() < X_ROT_RATE) {
    Spin = (Spin + 3) % 6;
    RecalcExtSites();
    //remember that if A and B activation is't simultaneous is_activeA and is_activeB must swap
  }

  return true;
}

void TParticle::RandomizePosition() {

// Randomize initial coordinates of the Central Site
  int yc;
  int xc = randM(Lx);
// On our triangular lattice, (x+y) must be always even
  if (xc % 2 == 0)
    yc = (randM(Ly) * 2) % Ly;
  else
    yc = (randM(Ly) * 2 + 1) % Ly;

  CSite = TSite(xc, yc);

}

void TParticle::RandomizeOrientation() {
  Spin = randM(6);
  RecalcExtSites();
}

void TParticle::RecalcExtSites() {
  RSite = TSite((CSite.x + dx[Spin] + Lx) % Lx, (CSite.y + dy[Spin] + Ly) % Ly);
  LSite = TSite((CSite.x - dx[Spin] + Lx) % Lx, (CSite.y - dy[Spin] + Ly) % Ly);
}

#define PARTICLE_LOGGING false

bool TParticle::CheckJoinWithCSite(TParticle &other) {

  if (other.mob == MobState::FREE) return false;

  if (CSite == other.RSite && is_activeA && other.is_freeR) {

    if (other.Spin == (Spin + 3) % 6) {
      //whit a rate DL2YL_RATE, switch to YLA
      if (ranMT() < DL2YL_RATE) {
        Spin = (Spin - 1 +6) % 6;
        YLA(other);
#if PARTICLE_LOGGING
          std::cout << "YL at his A of " << *this << " and " << other << std::endl;
#endif
        return true;
      }
      //otherwise, check also the other site and if possible realize DLAs
      if (other.is_activeA && is_freeR) {
        DLAs(other);
#if PARTICLE_LOGGING
        std::cout << "DL at their A's of " << *this << " and " << other << std::endl;
#endif
        return true;
      }
    }

    //YL at my A
    if (other.Spin == (Spin + 4) % 6) {
      YLA(other);
#if PARTICLE_LOGGING
      std::cout << "YL at his A of " << *this << " with " << other << std::endl;
#endif
      return true;
    }
  }

  if (CSite == other.LSite && is_activeB && other.is_freeL) {
    //DL at Bs
    if (other.Spin == (Spin + 3) % 6) {
      //whit a rate DL2YL_RATE, switch to YLB
      if (ranMT() < DL2YL_RATE) {
        Spin = (Spin + 1) % 6;
        YLB(other);
#if PARTICLE_LOGGING
          std::cout << "YL at his B of " << *this << " and " << other << std::endl;
#endif
        return true;
      }
      //otherwise, check also the other site and if possible realize DLBs
      if (is_freeL && other.is_activeB) {
        DLBs(other);
#if PARTICLE_LOGGING
        std::cout << "DL at their B's of " << *this << " and " << other << std::endl;
#endif
        return true;
      }
    }
    //YL at my B
    if (other.Spin == (Spin + 2) % 6) {
      YLB(other);
#if PARTICLE_LOGGING
      std::cout << "YL at other A of " << *this << " with " << other << std::endl;
#endif
      return true;
    }
  }
  return false;
}

bool TParticle::CheckJoinWithRSite(TParticle &other) {

  if (other.mob == MobState::FREE) return false;

  //YLR (at other A)
  if (RSite == other.CSite && other.Spin == (Spin + 2) % 6 && is_freeR && other.is_activeA) {
    YLR(other);
#if PARTICLE_LOGGING
    std::cout << "YL at other A of " << *this << " with " << other << std::endl;
#endif
    return true;
  }
  return false;
}

bool TParticle::CheckJoinWithLSite(TParticle &other) {

  if (other.mob == MobState::FREE) return false;
  //YLL (at other B)
  if (LSite == other.CSite && other.Spin == (Spin + 4) % 6 && is_freeL && other.is_activeB) {
    YLL(other);
#if PARTICLE_LOGGING
    std::cout << "YL at other B of " << *this << " with " << other << std::endl;
#endif
    return true;
  }
  return false;
}

void TParticle::DLAs(TParticle &other) {
  mob = MobState::BLOCKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[2] = other.Index;
  other.LinkedWith[3] = Index;
  LinkedWith[3] = other.Index;
  other.LinkedWith[2] = Index;
  is_activeA = false;
  is_freeR = false;
  other.is_freeR = false;
  other.is_activeA = false;
  Lattice->nDL++;
}

void TParticle::DLBs(TParticle &other) {
  mob = MobState::BLOCKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[0] = other.Index;
  other.LinkedWith[1] = Index;
  LinkedWith[1] = other.Index;
  other.LinkedWith[0] = Index;
  is_activeB = false;
  is_freeL = false;
  other.is_freeL = false;
  other.is_activeB = false;
  Lattice->nDL++;
}

void TParticle::YLL(TParticle &other) {
  mob = MobState::LINKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[0] = other.Index;
  other.LinkedWith[1] = Index;
  is_freeL = false;
  other.is_activeB = false;
  Lattice->nYL++;
}

void TParticle::YLB(TParticle &other) {
  mob = MobState::LINKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[1] = other.Index;
  other.LinkedWith[0] = Index;
  is_activeB = false;
  other.is_freeL = false;
  Lattice->nYL++;
}

void TParticle::YLA(TParticle &other) {
  mob = MobState::LINKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[2] = other.Index;
  other.LinkedWith[3] = Index;
  is_activeA = false;
  other.is_freeR = false;
  Lattice->nYL++;
}

void TParticle::YLR(TParticle &other) {
  mob = MobState::LINKED;
  other.mob = MobState::BLOCKED;
  LinkedWith[3] = other.Index;
  other.LinkedWith[2] = Index;
  is_freeR = false;
  other.is_activeA = false;
  Lattice->nYL++;
}

bool TParticle::CheckClose() {
    bool Closed= false;
  if (!is_freeL) {
    if (CheckCloseYLL(Lattice->GetParticle(LinkedWith[0]))) Closed=true;
  }
  if (!is_activeB) {
    if (CheckCloseYLB(Lattice->GetParticle(LinkedWith[1]))) Closed=true;
  }
  if (!is_activeA) {
    if (CheckCloseYLA(Lattice->GetParticle(LinkedWith[2]))) Closed=true;
  }
  if (!is_freeR) {
    if (CheckCloseYLR(Lattice->GetParticle(LinkedWith[3]))) Closed=true;
  }
  return Closed;
}

bool TParticle::CheckCloseYLL(TParticle &pPart) {
  if (!is_activeB || !pPart.is_freeL) return false;
  if (ranMT() > CLO_TRESH) return false;

  ClearParticlePosition();

  CSite.Translate(dx[(Spin + 2) % 6], dy[(Spin + 2) % 6]);
  Spin = (Spin + 1) % 6;
  RecalcExtSites();
  is_activeB = false;
  LinkedWith[1] = pPart.Index;
  pPart.is_freeL = false;
  pPart.LinkedWith[0] = Index;
  mob = MobState::BLOCKED;
  pPart.mob = MobState::BLOCKED;

  SetParticlePosition();

  Lattice->nYL--;
  Lattice->nDL++;
#if PARTICLE_LOGGING
  std::cout << "Closing! Of " << *this << " over " << pPart << std::endl;
#endif
  return true;
}

bool TParticle::CheckCloseYLB(TParticle &pPart) {
  if (!is_freeL || !pPart.is_activeB) return false;
  if (ranMT() > CLO_TRESH) return false;

  ClearParticlePosition();

  Spin = (Spin - 1 + 6) % 6;
  RecalcExtSites();
  is_freeL = false;
  LinkedWith[0] = pPart.Index;
  pPart.is_activeB = false;
  pPart.LinkedWith[1] = Index;
  mob = MobState::BLOCKED;
  pPart.mob = MobState::BLOCKED;

  SetParticlePosition();

  Lattice->nYL--;
  Lattice->nDL++;
#if PARTICLE_LOGGING
  std::cout << "Closing! Of " << *this << " over " << pPart << std::endl;
#endif
  return true;
}

bool TParticle::CheckCloseYLA(TParticle &pPart) {
  if (!is_freeR || !pPart.is_activeA) return false;
  if (ranMT() > CLO_TRESH) return false;

  ClearParticlePosition();

  Spin = (Spin + 1) % 6;
  RecalcExtSites();
  is_freeR = false;
  LinkedWith[3] = pPart.Index;
  pPart.is_activeA = false;
  pPart.LinkedWith[2] = Index;
  mob = MobState::BLOCKED;
  pPart.mob = MobState::BLOCKED;

  SetParticlePosition();

  Lattice->nYL--;
  Lattice->nDL++;
#if PARTICLE_LOGGING
  std::cout << "Closing! Of " << *this << " over " << pPart << std::endl;
#endif
  return true;
}

bool TParticle::CheckCloseYLR(TParticle &pPart) {
  if (!is_activeA || !pPart.is_freeR) return false;  //first check if there are closing conditions
  if (ranMT() > CLO_TRESH) return false; //if there are, close with a rate CLO_RATE

  ClearParticlePosition();

  CSite.Translate(dx[(Spin + 1) % 6], dy[(Spin + 1) % 6]);
  Spin = (Spin - 1 + 6) % 6;
  RecalcExtSites();
  is_activeA = false;
  LinkedWith[2] = pPart.Index;
  pPart.is_freeR = false;
  pPart.LinkedWith[3] = Index;
  mob = MobState::BLOCKED;
  pPart.mob = MobState::BLOCKED;

  SetParticlePosition();

  Lattice->nYL--;
  Lattice->nDL++;
#if PARTICLE_LOGGING
  std::cout << "Closing! Of " << *this << " over " << pPart << std::endl;
#endif
  return true;
}

void TParticle::SetParticlePosition() {
//Set Site by Site through the pointer at the shared Lattice
  Lattice->SetSitePosition(CSite, Index);
  Lattice->SetSitePosition(LSite, Index);
  Lattice->SetSitePosition(RSite, Index);
}

void TParticle::ClearParticlePosition() {
//Clear Site by Site through the pointer at the shared Lattice
  Lattice->ClearSitePosition(CSite, Index);
  Lattice->ClearSitePosition(LSite, Index);
  Lattice->ClearSitePosition(RSite, Index);
}

std::ostream &operator<<(std::ostream &os, const TParticle::MobState &me) {
  switch (me) {
    case TParticle::MobState::FREE:return os << "Free";
    case TParticle::MobState::LINKED :return os << "Linked";
    case TParticle::MobState::BLOCKED:return os << "Blocked";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const TParticle &me) {
  os << "Particle " << me.Index << " CS:" << me.CSite << " LS:" << me.LSite << " RS:" << me.RSite;
  os << " Spin:" << me.Spin << me.mob << " Parts Linked in L-B-A-R: " << me.LinkedWith[0] << "/" << me.LinkedWith[1] <<
     "/" << me.LinkedWith[2] << "/" << me.LinkedWith[3] << " ";
  return os;
}

void TParticle::CheckBorder() {
 if (CSite.x < 2 || CSite.x > (Lx- 3) || CSite.y < 1 || CSite.y > (Ly-2) ) Lattice->OutofGrid=true;
}


bool TParticle::CheckAggregate(TSite pSite) {
    if ((Lattice->GetSiteIndexes(pSite)).empty()) {return false;}
    return true;
}


