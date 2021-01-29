#include "TParticle.h"

TParticle::TParticle(int pIndex):Index(pIndex)/* se aggiungo :is_freeL(true) lo inizializza direttamente vero*/
{
RandomizePosition();
RandomizeOrientation();
is_freeL=true;
is_freeR=true;
is_activeA=true;
is_activeB=true;
mob=MobState::FREE;
}

/*TParticle::TParticle(const TParticle & o) {
Index=o.Index;
CSite=o.CSite;
RSite=o.RSite;
LSite=o.LSite;
Spin=o.Spin;
is_freeL=o.is_freeL;
is_freeR=o.is_freeR;
is_activeA=o.is_activeA;
is_activeB=o.is_activeB;
mob=o.mob;
};*/

TParticle::TParticle(TSite pSite, int pSpin, int pIndex):Index(pIndex)
{
CSite=pSite;
Spin=pSpin;
RecalcExtSites();
is_freeL=true;
is_freeR=true;
is_activeA=true;
is_activeB=true;
mob=MobState::FREE;
}

TParticle::~TParticle()
{
    //dtor
}

void TParticle::Evolve()
{

//if (!is_activeA) TryActivateA();
//if (!is_activeB) TryActivateB();

    switch (mob)
    {   case MobState::FREE:
        {

        ClearParticlePosition();
        FreeMove();

        for  (int i : Lattice->GetSiteIndexes(CSite))
        {
            if (i!=Index)
            {
                CheckJoinWithCSite(Lattice->GetParticle(i));
            }
        }

        for  (int i : Lattice->GetSiteIndexes(RSite))
        {
            if (i!=Index)
            {
                CheckJoinWithRSite(Lattice->GetParticle(i));
            }
        }

        for  (int i : Lattice->GetSiteIndexes(LSite))
        {
            if (i!=Index)
            {
                CheckJoinWithLSite(Lattice->GetParticle(i));
            }
        }

        SetParticlePosition();

        break;
        }

        case MobState::YLA:
        {
            ChekCloseYLA(Lattice->GetParticle(LinkedWith));
        break;
        }

        case MobState::YLB:
        {
            ChekCloseYLB(Lattice->GetParticle(LinkedWith));
        break;
        }

        case MobState::YLoA:
        {
            ChekCloseYLoA(Lattice->GetParticle(LinkedWith));
        break;
        }



    }
}

void TParticle::TryActivateA()
{
    if (ranMT()>ActivationTreshold) is_activeA=true;
}

void TParticle::TryActivateB()
{
    if (ranMT()>ActivationTreshold) is_activeB=true;
}


void TParticle::FreeMove()
{

    // Transalte CSite in one of the 6 neighbours Sites chosen randomly
    int tr=randM(6);
    CSite.Translate(dx[tr],dy[tr]);
    // and by consequence LSite and RSite by the same dx and dy
    LSite.Translate(dx[tr],dy[tr]);
    RSite.Translate(dx[tr],dy[tr]);

    // Rotate randomly clockwise or anticlockwise

    int r=randM(2); //by now, no possibility of rest

    //Change Orientation
    Spin=(Spin+R[r]+6)%6;

    RecalcExtSites();

}

void TParticle::RandomizePosition()
{

// Randomize initial coordinates of the Central Site
int yc;
int xc=randM(L);
// On our triangular lattice, (x+y) must be always even
if (xc%2==0)
    yc=(randM(L)*2)%L;
else
    yc=(randM(L)*2+1)%L;

CSite=TSite(xc,yc);

}

void TParticle::RandomizeOrientation()
{
Spin=randM(6);
RecalcExtSites();
}

void TParticle::RecalcExtSites()
{
RSite=TSite((CSite.x+dx[Spin]+L)%L,(CSite.y+dy[Spin]+L)%L);
LSite=TSite((CSite.x-dx[Spin]+L)%L,(CSite.y-dy[Spin]+L)%L);
}

void TParticle::CheckJoinWithCSite(TParticle &other)
{

if (other.mob==MobState::FREE) return;

    if (CSite==other.RSite){
        //DL at As
        if (other.Spin==(Spin+3)%6 && is_activeA && is_freeR && other.is_freeR && other.is_activeA) {
            mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
            is_activeA=false; is_freeR=false; other.is_freeR=false; other.is_activeA=false;
            //std::cout << "DL at their A's of " << *this << " and " << other << std::endl;
            return;
        }
        //YL at my A
        if (other.Spin==(Spin+4)%6 && is_activeA && other.is_freeR){
            mob=MobState::YLA; other.mob=MobState::BLOCKED; LinkedWith=other.Index;
            is_activeA=false; other.is_freeR=false;
            //std::cout << "YL at his A of " << *this << " with " << other << std::endl;
            return;
        }
    }

    if (CSite==other.LSite){
        //DL at Bs
        if (other.Spin==(Spin+3)%6 && is_activeB && is_freeL && other.is_freeL && other.is_activeB) {
            mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
            is_activeB=false; is_freeL=false; other.is_freeL=false; other.is_activeB=false;
            //std::cout << "DL at their B's of " << *this << " and " << other << std::endl;
            return;
        }
        //YL at my B
        if (other.Spin==(Spin+2)%6 && is_activeB && other.is_freeL){
            mob=MobState::YLB; other.mob=MobState::BLOCKED; LinkedWith=other.Index;
            is_activeB=false; other.is_freeL=false;
            //std::cout << "YL at other A of " << *this << " with " << other << std::endl;
            return;

        }
    }
}

void TParticle::CheckJoinWithRSite(TParticle &other)
{

if (other.mob==MobState::FREE) return;

    if (RSite==other.CSite){
        //YL at other A
        if (other.Spin==(Spin+2)%6 && is_freeR && other.is_activeA){
            mob=MobState::YLoA; other.mob=MobState::BLOCKED; LinkedWith=other.Index;
            is_freeR=false; other.is_activeA=false;
            //std::cout << "YL at other A of " << *this << " with " << other << std::endl;

        }
    }
}

void TParticle::CheckJoinWithLSite(TParticle &other)
{

if (other.mob==MobState::FREE) return;

        if (LSite==other.CSite){
        //YL at other B
        if (other.Spin==(Spin+4)%6 && is_freeL && other.is_activeB){
            mob=MobState::YLoB; other.mob=MobState::BLOCKED; LinkedWith=other.Index;
            is_freeL=false; other.is_activeB=false;
            //std::cout << "YL at other B of " << *this << " with " << other << std::endl;
        }
    }
}

void TParticle::ChekCloseYLA(TParticle &other)
{
    if (!is_freeR || !other.is_activeA) return

    ClearParticlePosition();

    mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
    Spin=(Spin+1)%6; RecalcExtSites();
    is_freeR=false; other.is_activeA=false;

    SetParticlePosition();

    //std::cout << "Closing! Of " << *this << " over " << other << std::endl;

}

void TParticle::ChekCloseYLB(TParticle &other)
{
    if (!is_freeL || !other.is_activeB) return

    ClearParticlePosition();

    mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
    Spin=(Spin-1)%6; RecalcExtSites();
    is_freeL=false; other.is_activeB=false;

    SetParticlePosition();

    //std::cout << "Closing! Of " << *this << " over " << other << std::endl;

}

void TParticle::ChekCloseYLoA(TParticle &other)
{
    if (!is_activeA || !other.is_freeR) return

    ClearParticlePosition();

    mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
    CSite=other.RSite; Spin=(Spin-1)%6; RecalcExtSites();
    is_activeA=false; other.is_freeR=false;

    SetParticlePosition();

    //std::cout << "Closing! Of " << *this << " over " << other << std::endl;

}

void TParticle::ChekCloseYLoB(TParticle &other)
{
    if (!is_activeB || !other.is_freeL) return

    ClearParticlePosition();

    mob=MobState::BLOCKED; other.mob=MobState::BLOCKED;
    CSite=other.LSite; Spin=(Spin+1)%6; RecalcExtSites();
    is_activeB=false; other.is_freeL=false;

    SetParticlePosition();

    //std::cout << "Closing! Of " << *this << " over " << other << std::endl;

}

void TParticle::SetParticlePosition()
{
//Set Site by Site through the pointer at the shared Lattice
Lattice->SetSitePosition(CSite,Index);
Lattice->SetSitePosition(LSite,Index);
Lattice->SetSitePosition(RSite,Index);
}

void TParticle::ClearParticlePosition()
{
//Clear Site by Site through the pointer at the shared Lattice
Lattice->ClearSitePosition(CSite,Index);
Lattice->ClearSitePosition(LSite,Index);
Lattice->ClearSitePosition(RSite,Index);
}

std::ostream& operator <<(std::ostream& os, const TParticle::MobState& me)
{
switch (me)
    {
    case TParticle::MobState::FREE: return os << "Free";
    case TParticle::MobState::YLA : return os << "YLA";
    case TParticle::MobState::YLB : return os << "YLB";
    case TParticle::MobState::YLoA : return os << "YLoA";
    case TParticle::MobState::YLoB : return os << "YLoB";
    case TParticle::MobState::BLOCKED: return os << "Blocked";
    }
}

std::ostream& operator <<(std::ostream& os, const TParticle& me)
{
os << "Particle " << me.Index << " CS:" << me.CSite <<  " LS:" << me.LSite << " RS:" << me.RSite;
os << " Spin:" << me.Spin << " Bools L-a-b-R: " << me.is_freeL << me.is_activeA << me.is_activeB << me.is_freeR << " " << me.mob;
return os;
}  //passo l'indirizzo ma non può modificare la variabile (const)
