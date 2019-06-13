#include <cstdio>

#include "../include/Program.h"

Program::Program(Vet c, Vet b, VarVet x, CoefMatrix A){
  this->c = c;
  this->b = b;
  this->x = x;
  this->A = A;
  this->optimal = false;

  verifyValidForm();
}

void Program::printProgram(){
  if(!this->isValid()){
    printf("Program is invalid!\n");
    return;
  }

  printf("max ");
  printLineMult(c, x);

  printf("\n\nSubject To: \n");
  for(int i = 0; i < b.size(); i++){
    printLineMult(A[i], x);

    printf(" <= %c%.2f\n", (b[i] < 0) ? '-' : ' ', dAbs(b[i]));
  }

  printf("%s", x[0].name.c_str());
  for(int i = 1; i < x.size(); i++)
    printf(", %s", x[i].name.c_str());

  printf(" >= 0\n");
}

void Program::optimize(){
  //TODO: This is the main stuff
  if(this->isOptimal() || !this->isValid())
    return;

}

double Program::dAbs(double d){
  return (d < 0) ? -1 * d : d;
}

void Program::printLineMult(Vet v, VarVet x){
  if(v[0] < 0)
    printf("- ");
  printf("%.2f%s", dAbs(v[0]), x[0].name.c_str());
  for(int i = 1; i < x.size(); i++)
    printf(" %c %.2f%s", (v[i] < 0) ? '-' : '+', dAbs(v[i]), x[i].name.c_str());
}

void Program::verifyValidForm(){
  bool aMatrixOk = true;
  int columns = x.size();

  for(auto row : A)
    if(row.size() != columns)
      aMatrixOk = false;

  this->valid = (x.size() > 0 && c.size() == x.size() && A.size() == b.size()) && aMatrixOk;
}
