#include <cstdio>
#include <string>
#include <vector>

#include "../include/Program.h"

int main(int argc, char* argv[]){

  Vet c = {3, 4, -2};
  Vet b = {-3, 8, 9};
  VarVet x;
  Var temp;
  temp.name = "x1";
  temp.real = true;
  x.push_back(temp);
  temp.name = "x2";
  temp.real = true;
  x.push_back(temp);
  temp.name = "x3";
  temp.real = true;
  x.push_back(temp);

  CoefMatrix A;
  Vet tempVet;

  tempVet.clear();
  tempVet.push_back(2.5);
  tempVet.push_back(1.5);
  tempVet.push_back(4.6);
  A.push_back(tempVet);
  tempVet.clear();
  tempVet.push_back(4.2);
  tempVet.push_back(0.4);
  tempVet.push_back(-2.4);
  A.push_back(tempVet);
  tempVet.clear();
  tempVet.push_back(5.1);
  tempVet.push_back(9.2);
  tempVet.push_back(4.8);
  A.push_back(tempVet);

  Program p(c, b, x, A);

  p.printProgram();

  return 0;
}
