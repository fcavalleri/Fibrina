#ifndef TSITE_H
#define TSITE_H
#include <iostream>

//! Represent one of the Sites composing a Particle
class TSite
{
    public:
        //Constructors & Distructors
        TSite();
        TSite(int x, int y);
        virtual ~TSite();

        //Class Members
        //! Linear dimension of the grid
        inline static int L;
        //! Site coordinates.
        //! Be careful: this are lattice coordinate that sould be rescalated in real world by...
        int x, y;

        //Class Methods
        //! Translate a Site by dx and dy
        void Translate(int dx,int dy);

        //!
        friend std::ostream& operator <<(std::ostream& os, const TSite& me);  //passo l'indirizzo ma non può modificare la variabile (const)

    protected:

    private:
};

//Define a class operator
bool operator== (const TSite a, const TSite b);

#endif // TSITE_H
