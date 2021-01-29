#include "TLattice.h"

TLattice::TLattice(int pL) : L(pL), Grid(L, std::vector<GridElement>(L)), N(0)  //Initialize
{
    // Set static parameters for TParticle and TSite classes
    TParticle::L = pL;
    TSite::L = pL;
    TParticle::Lattice = this; //this è il puntatore all'istanza corrente della classe

}


TLattice::~TLattice() {
    //dtor
}

void TLattice::RandomFill(int pN) {

//Fill with a cycle the vector Parts
    for (int i = 0; i < pN; i++) {
        TParticle x(N);        //Create a Tparticle-Type variable named x Setting x Index as current N
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
    Parts[N - 1].ClearParticlePosition();

    Parts[N - 1].CSite.x = L/2;
    Parts[N - 1].CSite.y = L/2;
    Parts[N - 1].Spin = 0;
    Parts[N - 1].LSite.x = L/2 -2;
    Parts[N - 1].LSite.y = L/2;
    Parts[N - 1].RSite.x = L/2 +2;
    Parts[N - 1].RSite.y = L/2;
    Parts[N - 1].is_freeL = true;
    Parts[N - 1].is_freeR = true;
    Parts[N - 1].is_activeA = true;
    Parts[N - 1].is_activeB = true;
    Parts[N - 1].mob = TParticle::MobState::BLOCKED;


    Parts[N - 1].SetParticlePosition();
}

void TLattice::SetGraphicContext(sf::RenderWindow *pApp) {
    app = pApp;
}

void TLattice::Evolve() {
    //For N times, chose a random particle from Parts vector and Evolve it
    for (int i = 0; i < N; i++) {

        Parts[randM(N)].Evolve();

    }

    /*for (auto &i : Parts){
    //It's necessary another cycle to draw all particles, no matters if they have moved or not in this moment
        sf::Vertex monomeroC=sf::Vertex(sf::Vector2f(i.CSite.x,i.CSite.y), sf::Color::Red);
        app->draw(&monomeroC,1,sf::Points);

        sf::Vertex monomeroS=sf::Vertex(sf::Vector2f(i.LSite.x,i.LSite.y), sf::Color::White);
        app->draw(&monomeroS,1,sf::Points);

        sf::Vertex monomeroD=sf::Vertex(sf::Vector2f(i.RSite.x,i.RSite.y), sf::Color::White);
        app->draw(&monomeroD,1,sf::Points);
        }*/
}

void TLattice::SetSitePosition(TSite &pSite, int pIndex) {
    //Gives at the int element of the Grid struct the value pIndex
    Grid[pSite.x][pSite.y].push_back(pIndex);
    //Gives at the bool element of the Grid struct the value is_central given by pSite
    //Grid[pSite.x*L+pSite.y].is_central=pSite.is_central;
}

void TLattice::ClearSitePosition(TSite &pSite, int pIndex)//////////////////////////////////////
{
    //Gives at the int element of the Grid struct the value -1. The bool value doesn't matter
    GridElement &cell = Grid[pSite.x][pSite.y];
    cell.erase(std::find(cell.begin(), cell.end(), pIndex));
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
