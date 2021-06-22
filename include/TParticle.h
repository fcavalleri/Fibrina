#ifndef TPARTICLE_H
#define TPARTICLE_H
#include <cmath>
#include "RandUtils.h"
#include "TSite.h"
#include "TLattice.h"
#include <iostream>

//class forward declaration
class TLattice;

//! Represent a Particle made up of 3 Sites
class TParticle {
public:
  //! Rates for traslation and rotations
  inline static double ZY_ROT_RATE;
  inline static double X_ROT_RATE;
  inline static double TRANSL_RATE;
  //! Treshold of activation for central sites A e B
  inline static double ACT_TRESH;
  inline static double CLO_TRESH;
  inline static double DL2YL_RATE;
  //! Pointer to the Lattice shared by all Parts
  inline static TLattice *Lattice;

  //Creator & Distructor
  TParticle(int pIndex);
  TParticle(int pIndex, TSite pCSite, int pSpin);
  //Copy Constructor
  TParticle(const TParticle &o) = default;
  virtual ~TParticle();

  //Class Members
  //! Index of the particle
  const int Index;

  //! Monomers Sites that composes a particle
  TSite CSite, LSite, RSite;
  //! Orientation of the particle
  int Spin;
  //! Site Activity
  bool is_freeL;
  bool is_freeR;
  bool is_activeA;
  bool is_activeB;
  //! State of the monomer
  enum class MobState { FREE, LINKED, BLOCKED };
  MobState mob;
  //! Index of the particle with I am linked
  int LinkedWith[4];

  //Class Methods
  //! Make a step forward in time
  bool Evolve();
  //Setter and Getter
  //! Set in the Lattice the Index of the particle in his Sites position
  void SetParticlePosition();
  //! Clear the Lattice where the particle was before moving
  void ClearParticlePosition();

  friend std::ostream &operator<<(std::ostream &os,
                                  const TParticle &me);  //passo l'indirizzo ma non può modificare la variabile (const)

protected:

private:
  //Class Statics Values
  //! Possibility of X coordinates translation on a triangular lattice
  static constexpr int dx[6] = {2, 1, -1, -2, -1, 1};  //IN ORDINE!!
  //! Possibility of Y coordinates translation on a triangular lattice
  static constexpr int dy[6] = {0, 1, 1, 0, -1, -1};
  //! Possibility of a planar rotation (clockwise or anticlockwise)
  static constexpr int R[2] = {-1, 1};

  //Class Internal Methods
  //! Give random position at the CSite of the monomer
  void RandomizePosition();
  //! Give random orientation at the present monomer
  void RandomizeOrientation();
  //! Set the RSite & LSite
  void RecalcExtSites();
  //! Movement of a FREE particle. If not meet the aggregates, moves free
  bool FreeMove();

  //! Try to activate central sites A & B;
  void TryActivateAB();

  //! If it's possible an aggregation with the input particle, occurs
  bool CheckJoinWithCSite(TParticle &pPart);
  bool CheckJoinWithLSite(TParticle &pPart);
  bool CheckJoinWithRSite(TParticle &pPart);

  //! Link realization sintaxis
  void DLAs(TParticle &pPart);
  void DLBs(TParticle &pPart);
  void YLL(TParticle &pPart);
  void YLB(TParticle &pPart);
  void YLA(TParticle &pPart);
  void YLR(TParticle &pPart);

  //! If it's possible a closure over the input particle, occurs
  bool CheckClose();
  bool CheckCloseYLA(TParticle &pPart);
  bool CheckCloseYLB(TParticle &pPart);
  bool CheckCloseYLR(TParticle &pPart);
  bool CheckCloseYLL(TParticle &pPart);

  //! Check if a new added monomer in the aggregate is on the border
  void CheckBorder();

  bool CheckAggregate(TSite pSite);
};

#endif // TPARTICLE_H
