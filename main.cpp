#include "NAT.h"

//very simple nat punch through main
int main()
{
  //start a nat server on port 1490
  NAT nat(1490);
  while(1)
  {
    nat.Update();
  }
}