#include <stdio.h>

struct Kutya
{
    const int fulek = 2;
    mutable int labak = 4;
    int szemek = 2;
};


int main(int argc, char const *argv[])
{
    // mutable
    const Kutya morzsa;

    printf("morzsa.labak=%d\n", morzsa.labak);
    morzsa.labak = 2;  // hiaba const az objektum, megis valtoztathato (mutable)
    // morzsa.szemek = 1; // nem lehet
    printf("morzsa.labak=%d\n", morzsa.labak);


    // const cast
    const Kutya maszat;
    printf("maszat.fulek=%d\n", maszat.fulek);

    int* ptr_fulek = const_cast<int*>(&maszat.fulek);
    *ptr_fulek = 4;
    printf("maszat.fulek=%d\n", maszat.fulek);


    // const + pointer
    Kutya bloki;
    printf("bloki.labak=%d\n", bloki.labak);

    // a mutato allando
    int* const vmi = &bloki.labak;
    *vmi = 3;
    // vmi = &bloki.szemek; // nem lehet
    printf("bloki.labak=%d\n", bloki.labak);

    // az ertek allando
    const int* vmi2 = &bloki.labak;
    // *vmi2 = 3; // nem lehet
    vmi2 = &bloki.szemek;

    return 0;
}


