#include "TParticle.h"

TParticle::TParticle(int pIndex) : Index(pIndex), is_freeL(true), is_freeR(true), LinkedWith{-1,-1,-1,-1}/* se aggiungo :is_freeL(true) lo inizializza direttamente vero*/
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

TParticle::TParticle(int pIndex, TSite pSite, int pSpin):Index(pIndex),CSite(pSite), is_freeL(true), is_freeR(true), LinkedWith{-1,-1,-1,-1}, Spin(pSpin)
{
RecalcExtSites();
is_activeA=false;
is_activeB=false;
mob=MobState::FREE;
}

TParticle::~TParticle() {
    //dtor
}

bool TParticle::Evolve() {

    bool JustJoined=false;
    nYL=0;
    nDL=0;

    switch (mob) {
        case MobState::FREE: {

            TryActivateAB();

            ClearParticlePosition();
            FreeMove();

            for (int i : Lattice->GetSiteIndexes(CSite)) {
                if (i != Index) {
                    if (CheckJoinWithCSite(Lattice->GetParticle(i))) JustJoined=true;
                }
            }

            for (int i : Lattice->GetSiteIndexes(RSite)) {
                if (i != Index) {
                    if (CheckJoinWithRSite(Lattice->GetParticle(i))) JustJoined=true;
                }
            }

            for (int i : Lattice->GetSiteIndexes(LSite)) {
                if (i != Index) {
                    if (CheckJoinWithLSite(Lattice->GetParticle(i))) JustJoined=true;
                }
            }

            SetParticlePosition();

            break;
        }

        case MobState::LINKED: {
            if (LinkedWith[1] == -1 && LinkedWith[2] == -1) TryActivateAB();
            CheckClose();
            break;
        }

        case MobState::BLOCKED: {
        }
    }
    return JustJoined;
}

void TParticle::TryActivateAB() {
    if (ranMT() > ACT_TRESH) {is_activeA = true; is_activeB = true;}
}

void TParticle::FreeMove() {
    // Transalte CSite in one of the 6 neighbours Sites chosen randomly
    if (ranMT() < TRANSL_RATE) {

        int tr = randM(6);
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

bool TParticle::CheckJoinWithCSite(TParticle &other) {

    if (other.mob == MobState::FREE) return false;

    if (CSite == other.RSite && is_activeA && other.is_freeR) {

        if (other.Spin == (Spin + 3) % 6){
            //whit a rate DL2YL_RATE, switch to YLA
            if (ranMT()<DL2YL_RATE) {Spin = (Spin - 1) % 6; YLA(other); return true;}
            //otherwise, check also the other site and if possible realize DLAs
            if (other.is_activeA && is_freeR){
                DLAs(other);
                std::cout << "DL at their A's of " << *this << " and " << other << std::endl;
                return true;
            }
        }

        //YL at my A
        if (other.Spin == (Spin + 4) % 6){
            YLA(other);
            std::cout << "YL at his A of " << *this << " with " << other << std::endl;
            return true;
        }
    }

    if (CSite == other.LSite && is_activeB && other.is_freeL) {
        //DL at Bs
        if (other.Spin == (Spin + 3) % 6) {
            //whit a rate DL2YL_RATE, switch to YLB
            if (ranMT()<DL2YL_RATE) {Spin = (Spin + 1) % 6; YLB(other); return true;}
            //otherwise, check also the other site and if possible realize DLBs
            if (is_freeL && other.is_activeB) {
                DLBs(other);
                std::cout << "DL at their B's of " << *this << " and " << other << std::endl;
                return true;
            }
        }
        //YL at my B
        if (other.Spin == (Spin + 2) % 6) {
            YLB(other);
            std::cout << "YL at other A of " << *this << " with " << other << std::endl;
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
            std::cout << "YL at other A of " << *this << " with " << other << std::endl;
            return true;
    }
    return false;
}

bool TParticle::CheckJoinWithLSite(TParticle &other) {

    if (other.mob == MobState::FREE) return false;
    //YLL (at other B)
    if (LSite == other.CSite && other.Spin == (Spin + 4) % 6 && is_freeL && other.is_activeB) {
            YLL(other);
            std::cout << "YL at other B of " << *this << " with " << other << std::endl;
            return true;
    }
return false;
}

void TParticle::DLAs(TParticle &other){
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
    nDL+=1;
}

void TParticle::DLBs(TParticle &other){
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
    nDL+=1;
}

void TParticle::YLL(TParticle &other){
    mob = MobState::LINKED;
    other.mob = MobState::BLOCKED;
    LinkedWith[0] = other.Index;
    other.LinkedWith[1] = Index;
    is_freeL = false;
    other.is_activeB = false;
    nYL+=1;
}

void TParticle::YLB(TParticle &other){
    mob = MobState::LINKED;
    other.mob = MobState::BLOCKED;
    LinkedWith[1] = other.Index;
    other.LinkedWith[0] = Index;
    is_activeB = false;
    other.is_freeL = false;
    nYL+=1;
}
void TParticle::YLA(TParticle &other){
    mob = MobState::LINKED;
    other.mob = MobState::BLOCKED;
    LinkedWith[2] = other.Index;
    other.LinkedWith[3] = Index;
    is_activeA = false;
    other.is_freeR = false;
    nYL+=1;
}

void TParticle::YLR(TParticle &other){
    mob = MobState::LINKED;
    other.mob = MobState::BLOCKED;
    LinkedWith[3] = other.Index;
    other.LinkedWith[2] = Index;
    is_freeR = false;
    other.is_activeA = false;
    nYL+=1;
}


void TParticle::CheckClose() {
    if (!is_freeL) {
        if (ChekCloseYLL(Lattice->GetParticle(LinkedWith[0]))) {nYL-=1; nDL+=1;}
        return;
    }
    if (!is_activeB) {
        if (ChekCloseYLB(Lattice->GetParticle(LinkedWith[1]))) {nYL-=1; nDL+=1;}
        return;
    }
    if (!is_activeA) {
        if (ChekCloseYLA(Lattice->GetParticle(LinkedWith[2]))) {nYL-=1; nDL+=1;};
        return;
    }
    if (!is_freeR) {
        if (ChekCloseYLR(Lattice->GetParticle(LinkedWith[3]))) {nYL-=1; nDL+=1;};
        return;
    }
}


bool TParticle::ChekCloseYLL(TParticle &other) {
    if (!is_activeB || !other.is_freeL) return false;
    if (ranMT() > CLO_TRESH) return false;

    ClearParticlePosition();

    CSite.Translate(dx[(Spin+2)%6], dy[(Spin+2)%6]);
    Spin = (Spin + 1) % 6;
    RecalcExtSites();
    is_activeB = false; LinkedWith[1] = other.Index;
    other.is_freeL = false; other.LinkedWith[0] = Index;
    mob = MobState::BLOCKED;
    other.mob = MobState::BLOCKED;

    SetParticlePosition();

    std::cout << "Closing! Of " << *this << " over " << other << std::endl;

    return true;
}

bool TParticle::ChekCloseYLB(TParticle &other) {
    if (!is_freeL || !other.is_activeB) return false;
    if (ranMT() > CLO_TRESH) return false;

    ClearParticlePosition();

    Spin = (Spin - 1) % 6;
    RecalcExtSites();
    is_freeL = false; LinkedWith[0] = other.Index;
    other.is_activeB = false; other.LinkedWith[1] = Index;
    mob = MobState::BLOCKED;
    other.mob = MobState::BLOCKED;

    SetParticlePosition();

    std::cout << "Closing! Of " << *this << " over " << other << std::endl;

    return true;
}

bool TParticle::ChekCloseYLA(TParticle &other) {
    if (!is_freeR || !other.is_activeA) return false;
    if (ranMT() > CLO_TRESH) return false;

    ClearParticlePosition();

    Spin = (Spin + 1) % 6;
    RecalcExtSites();
    is_freeR = false; LinkedWith[3] = other.Index;
    other.is_activeA = false; other.LinkedWith[2] = Index;
    mob = MobState::BLOCKED;
    other.mob = MobState::BLOCKED;

    SetParticlePosition();

    std::cout << "Closing! Of " << *this << " over " << other << std::endl;

    return true;
}

bool TParticle::ChekCloseYLR(TParticle &other) {
    if (!is_activeA || !other.is_freeR) return false;  //first check if there are closing conditions
    if (ranMT() > CLO_TRESH) return false; //if there are, close with a rate CLO_RATE

    ClearParticlePosition();

    CSite.Translate(dx[(Spin+1)%6], dy[(Spin+1)%6]);
    Spin = (Spin - 1) % 6;
    RecalcExtSites();
    is_activeA = false; LinkedWith[2] = other.Index;
    other.is_freeR = false; other.LinkedWith[3] = Index;
    mob = MobState::BLOCKED;
    other.mob = MobState::BLOCKED;

    SetParticlePosition();

    std::cout << "Closing! Of " << *this << " over " << other << std::endl;

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

int TParticle::GetYL() {
return nYL;
}

int TParticle::GetDL() {
    return nDL;
}

std::ostream &operator<<(std::ostream &os, const TParticle::MobState &me) {
    switch (me) {
        case TParticle::MobState::FREE:
            return os << "Free";
        case TParticle::MobState::LINKED :
            return os << "Linked";
        case TParticle::MobState::BLOCKED:
            return os << "Blocked";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const TParticle &me) {
    os << "Particle " << me.Index << " CS:" << me.CSite << " LS:" << me.LSite << " RS:" << me.RSite;
    os << " Spin:" << me.Spin << me.mob << " Parts Linked in L-B-A-R: " << me.LinkedWith[0] <<"/"<< me.LinkedWith[1] <<
       "/" << me.LinkedWith[2]<< "/" << me.LinkedWith[3] << " ";
    return os;
}
