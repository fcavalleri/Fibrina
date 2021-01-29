#ifndef RANDUTILS_H
#define RANDUTILS_H
#include <math.h>

int randM(int M);
double ranMT(void);
void seedMT2(void);

class RandUtils
{
    public:
        RandUtils();
        virtual ~RandUtils();

    protected:

    private:
};

#endif // RANDUTILS_H
