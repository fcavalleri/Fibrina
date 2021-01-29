#ifndef TPARTICLE_H
#define TPARTICLE_H
#include <math.h>
#include "RandUtils.h"
#include "TSite.h"
#include "TLattice.h"
#include <iostream>

//"class forward declaration" da mettere quando 2 classi si includono a vicenda
class TLattice;

//! Represent a Particle made up of 3 Sites
class TParticle
{
    public:
        //Class Static Parameters
        //! Linear dimension of the grid
        inline static int L;
        //! Rates for traslation and rotations
        inline static double ZY_ROT_RATE;
        inline static double X_ROT_RATE;
        inline static double TRANSL_RATE;
        //! Treshold of activation for central sites A e B
        inline static double ActivationTreshold;
        //! Pointer to the Lattice shared by all Parts
        inline static TLattice *Lattice;

        //Creator & Distructor
        TParticle(int pIndex);
        TParticle(TSite pSite, int pSpin, int pIndex);
        //Costruttore per copia che garantisce la copia corretta di questa classe
        TParticle(const TParticle & o) = default;

        virtual ~TParticle();

        //Class Members
        //!
        const int Index;
        //! Monomers Sites
        TSite CSite, LSite, RSite;
        //! Orientation
        int Spin;
        //! Site Activity
        bool is_freeL;
        bool is_freeR;
        bool is_activeA;
        bool is_activeB;

        //! State of the monomer
        enum class MobState {FREE, YLA, YLB, YLoA, YLoB, BLOCKED};
        MobState mob;

        //! Index of the particle with I am linked
        int LinkedWith;

        //Class Methods
        //! Make a step forward in time
        void Evolve();
        void FreeMove();

        //! Try to activate central sites A & B;
        void TryActivateA();
        void TryActivateB();

        //! Check the possible neighbour to aggregation whit other particle
        void CheckJoinWithCSite(TParticle &pPart);
        void CheckJoinWithLSite(TParticle &pPart);
        void CheckJoinWithRSite(TParticle &pPart);

        //! Check the possible closure over other particle
        void ChekCloseYLA(TParticle &pPart);
        void ChekCloseYLB(TParticle &pPart);
        void ChekCloseYLoA(TParticle &pPart);
        void ChekCloseYLoB(TParticle &pPart);

        //Setter and Getter
        //!
        void SetParticlePosition();
        //!
        void ClearParticlePosition();

        friend std::ostream& operator <<(std::ostream& os, const TParticle& me);  //passo l'indirizzo ma non può modificare la variabile (const)

    protected:

    private:
        //Class Statics Values
        //! Possibility of X coordinates translation on a triangular lattice
        static constexpr int dx[6]={2,1,-1,-2,-1,1};  //IN ORDINE!!
        //! Possibility of Y coordinates translation on a triangular lattice
        static constexpr int dy[6]={0,1,1,0,-1,-1};
        static constexpr int R[2]={-1,1};

        //Class Internal Methods
        //! Give random position at the CSite of the monomer
        void RandomizePosition();

        //! Give random orientation at the present monomer
        void RandomizeOrientation();

        //! Set the RSite & LSite
        void RecalcExtSites();
};

#endif // TPARTICLE_H
