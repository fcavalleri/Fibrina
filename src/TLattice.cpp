#include "TLattice.h"

static constexpr int Lx = TSite::Lx, Ly = TSite::Ly;

TLattice::TLattice() :  /*Grid(Lx, std::vector<GridElement>(Ly)),*/
N(0), Nfix(0), nYL(0), nDL(0), OutofGrid(false) //Initialize
{
  // Set static parameters for TParticle and TSite classes
  TParticle::Lattice = this; //this è il puntatore all'istanza corrente della classe
}

TLattice::~TLattice() {
  //dtor
}

void TLattice::RandomFill(int pN) {

//Fill with a cycle the vector Parts
  for (int i = 0; i < pN; i++) {
    TParticle x{N};        //Create a Tparticle-Type variable named x Setting x Index as current N
    N++;                //Increment N by unit
    x.SetParticlePosition();
    Parts.push_back(x); //Put x in the last position of vector Parts
  }
}

/*void TLattice::AddPart(TParticle pPart)
{
    N++;                //Increment N by unit
    pPart.SetParticlePosition();
    Parts.push_back(pPart);
}*/

void TLattice::SetForDLA() {
  Parts[0].ClearParticlePosition();

  Parts[0].CSite.x = Lx / 2;
  Parts[0].CSite.y = Ly / 2;
  Parts[0].Spin = 0;
  Parts[0].LSite.x = Lx / 2 - 2;
  Parts[0].LSite.y = Ly / 2;
  Parts[0].RSite.x = Lx / 2 + 2;
  Parts[0].RSite.y = Ly / 2;
  Parts[0].is_freeL = true;
  Parts[0].is_freeR = true;
  Parts[0].is_activeA = true;
  Parts[0].is_activeB = true;
  Parts[0].mob = TParticle::MobState::BLOCKED;

  Parts[0].SetParticlePosition();

  Nfix=1;
}

bool TLattice::Evolve() {
  //For N times, chose a random particle from Parts vector and Evolve it
  for (int i = 0; i < N; i++) {
      int j=randM(N);
    if (Parts[j].Evolve()) {
        // Particles in polimer
        Nfix++;
        if (Nfix > (MAX_Nfix-1) || OutofGrid) return true;
        //Add new particle to preserve free monomer concentration
        //RandomFill(1);
    };
  }
    return false;
}

void TLattice::SetSitePosition(TSite &pSite, int pIndex) {
  //Gives at the int element of the Grid struct the value pIndex
  Grid[pSite.x][pSite.y].insert_safe(pIndex);
  //Gives at the bool element of the Grid struct the value is_central given by pSite
  //Grid[pSite.x*L+pSite.y].is_central=pSite.is_central;
}

void TLattice::ClearSitePosition(TSite &pSite, int pIndex) {
  //Gives at the int element of the Grid struct the value -1. The bool value doesn't matter
  GridElement &cell = Grid[pSite.x][pSite.y];
  cell.erase(pIndex);
  //prima era
  /*std::remove(Grid[pos].begin(),Grid[pos].end(),pIndex),Grid[pos].end()*/
}

TLattice::GridElement &TLattice::GetSiteIndexes(TSite &pSite) {
//if (Grid[pSite.x*L+pSite.y].size()==0) return -1
  return Grid[pSite.x][pSite.y];
}

TParticle &TLattice::GetParticle(int pIndex) {
//not a copy, the reference at that particle with index pIndex
  return Parts.at(pIndex);
}

void TLattice::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  for (auto &i : Parts) {
    if (i.mob != TParticle::MobState::FREE) {
    //if (!(i.CSite.x < 2 || i.CSite.x > (Lx- 3) || i.CSite.y < 1 || i.CSite.y > (Ly-2) )) {
          sf::Vertex monomer[] = {sf::Vertex(sf::Vector2f(i.LSite.x, i.LSite.y)),
                                  sf::Vertex(sf::Vector2f(i.RSite.x, i.RSite.y))};
          target.draw(monomer, 2, sf::Lines, states);
      }
    // Uncommento to draw also free monomers
    else {
        sf::Vertex monomerCM=sf::Vertex(sf::Vector2f(i.CSite.x,i.CSite.y), sf::Color::White);
        target.draw(&monomerCM,1,sf::Points);
    }
    //}
  }
}