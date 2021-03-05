#ifndef TLATTICE_H
#define TLATTICE_H
#include <memory>
#include "TParticle.h"
#include <vector>
#include "RandUtils.h"
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include "small_vector.hpp"

//"class forward declaration" da mettere quando 2 classi si includono a vicenda
class TParticle;

//! This class ...
class TLattice {
public:
  //! Constructor needs Lattice linear dimension (pL)
  TLattice();
  virtual ~TLattice();

  //Class Members
  //! We have only one istance of this class, so L and N are a members
  int N;
  //!
  int nYL, nDL;
  //! The Lattice own as a member the vector of All Parts
  std::vector<TParticle> Parts;

  //Class Methods
  //! Fill the Lattice whit pN Particles
  void RandomFill(int pN);
  //! Set for a DLA simulation
  void SetForDLA();

  //! Make a time step forwar for all the system
  void Evolve();

  //! The Lattice is composed by Grid Elements made up with an int (the Index of the Particle if present) and the bool Is_Central
  typedef mtx::small_vector<uint8_t> GridElement; // type def

  //Class Setter ang Getter Methods
  //! Put pIndex of the Index-Particle in the pSite.x and pSite.y position of the Lattice
  void SetSitePosition(TSite &pSite, int pIndex);
  //! Clear the pSite.x and pSite.y position of the Lattice (at the moment, pIndex is unused)
  void ClearSitePosition(TSite &pSite, int pIndex);
  //! Get the vector of indexes contained in the position of pSite
  GridElement &GetSiteIndexes(TSite &pSite);
  //! Get the particle with a site in pSite (if there is any particle here)
  TParticle &GetParticle(int pIndex);

protected:

private:

  //! Here we don't kwon yet N, so we initializate a pointer to a integer array
  // std::array<std::array<GridElement, TSite::Ly>, TSite::Lx> Grid;
  mtx::matrix<GridElement, TSite::Lx, TSite::Ly> Grid;

};

#endif // TLATTICE_H
