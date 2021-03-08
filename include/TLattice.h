#ifndef TLATTICE_H
#define TLATTICE_H
#include <memory>
#include "TParticle.h"
#include <vector>
#include "RandUtils.h"
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include "small_uint_set.hpp"

//"class forward declaration" da mettere quando 2 classi si includono a vicenda
class TParticle;

//! This class ...
class TLattice : public sf::Drawable {

public:
  //! Constructor
  TLattice();
  virtual ~TLattice();

  //Class Members
  //! Total number of particles
  int N;
  //! Number of particles in the polimer
  int Nfix;
  //! The Lattice own as a member the vector of All Parts
  std::vector<TParticle> Parts;
  //! Number of YL and DL links
  int nYL, nDL;
  //! Compute sum and squared sum of xs and ys to calc baricentre and square gyration radius of polimer
  double sumx, sumy, sumx2, sumy2;
  double Rg2;

  //Class Methods
  //! Fill the Lattice whit pN Particles
  void RandomFill(int pN);
  //! Set for a DLA simulation
  void SetForDLA();

  //! Make a time step forwar for all the system
  void Evolve();

  //! The Lattice is composed by Grid Elements made up with an int (the Index of the Particle if present) and the bool Is_Central
  //typedef mtx::small_vector<uint16_t> GridElement; // type def
  typedef mtx::small_uint_set<uint16_t,10> GridElement;

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
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:

  //! Matrix class manage positions of particles trought their indexes
  // std::array<std::array<GridElement, TSite::Ly>, TSite::Lx> Grid;
   mtx::matrix<GridElement, TSite::Lx, TSite::Ly> Grid;

};

#endif // TLATTICE_H
