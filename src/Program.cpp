#include <cstdio>
#include <string>

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

double Program::getObjectiveValue(){
  double result = 0;
  for(int i = 0; i < x.size(); i++)
    result = result + x[i].value * c[i];
  return result;
}

VarVet Program::getVars(){
  return this->x;
}

SolverResult Program::optimize(bool verbose){
  if(this->isOptimal())
    return OPTIMAL;
  if(!this->isValid())
    return INVALID;

  return simplex(verbose);
}

SolverResult Program::simplex(bool verbose){
  Var tempVar;
  VarVet base;

  for(int i = 0; i < x.size(); i++)
    x[i].value = 0.0f;
  for(int i = 0; i < b.size(); i++){
    tempVar.name = "w" + std::to_string(i+1);
    tempVar.value = b[i];
    x.push_back(tempVar);
    base.push_back(tempVar);
    c.push_back(0.0f);
    for(int j = 0; j < b.size(); j++){
      A[j].push_back((i == j) ? 1.0f : 0.0f);
    }
  }

  Vet tempC = c;
  CoefMatrix tempA = A;

  int in, out;

  for(int i = 0; i < x.size(); i++){
    for(int j = 0; j < b.size(); j++){
      tempA[j][i] = -tempA[j][i];
    }
  }

  bool phase1 = false;
  for(int i = 0; i < base.size(); i++)
    if(base[i].value < 0) phase1 = true;

  if(phase1){
    for(int i = 0; i < x.size(); i++)
      tempC[i] = 0.0f;

    tempVar.name = "x0";
    tempVar.value = 0.0f;
    x.push_back(tempVar);
    tempC.push_back(-1.0f);
    for(int j = 0; j < b.size(); j++){
      tempA[j].push_back(1.0f);
    }

    in = x.size() - 1;
    out = 0;
    for(int i = 1; i < base.size(); i++)
      if(base[i].value < base[out].value) out = i;

    if(verbose) printDict(tempC, tempA, base);

    //Pivot
    for(int i = 0; i < x.size(); i++){
      tempA[out][i] = -tempA[out][i];
    }
    base[out].name = x[in].name;
    base[out].value = -base[out].value;

    double mult;
    for(int i = 0; i < base.size(); i++){
      if(i == out) continue;
      mult = tempA[i][in];
      for(int j = 0; j < x.size(); j++)
        tempA[i][j] = tempA[i][j] + tempA[out][j] * mult;
      base[i].value = base[i].value + base[out].value * mult;
    }
    mult = tempC[in];
    for(int j = 0; j < x.size(); j++)
      tempC[j] = tempC[j] + tempA[out][j] * mult;

    if(verbose) printDict(tempC, tempA, base);

    //Next pivots
    bool shouldFinish = true;
    bool unbound;
    double mostRestrictive = 0;
    for(int i = 0; i < tempC.size(); i++)
      if(tempC[i] > 0) shouldFinish = false;
    while(!shouldFinish){
      in = 0;
      for(int i = 0; i < tempC.size(); i++)
        if(tempC[in] < tempC[i]) in = i;

      out = 0;
      unbound = true;
      for(int i = 0; i < base.size(); i++){
        if(tempA[i][in] < -0.00001){
          if(unbound){
            unbound = false;
            mostRestrictive = -base[i].value / tempA[i][in];
            out = i;
          }
          else{
            if(-base[i].value / tempA[i][in] < mostRestrictive){
              mostRestrictive = -base[i].value / tempA[i][in];
              out = i;
            }
          }
        }
      }

      double lineDivisor = -tempA[out][in];
      for(int i = 0; i < x.size(); i++){
        tempA[out][i] = tempA[out][i] / lineDivisor;
      }
      base[out].name = x[in].name;
      base[out].value = base[out].value / lineDivisor;

      for(int i = 0; i < base.size(); i++){
        if(i == out) continue;
        mult = tempA[i][in];
        for(int j = 0; j < x.size(); j++)
          tempA[i][j] = tempA[i][j] + tempA[out][j] * mult;
        base[i].value = base[i].value + base[out].value * mult;
      }
      mult = tempC[in];
      for(int j = 0; j < x.size(); j++)
        tempC[j] = tempC[j] + tempA[out][j] * mult;

      if(verbose) printDict(tempC, tempA, base);

      shouldFinish = true;
      for(int i = 0; i < tempC.size(); i++)
        if(tempC[i] > 0) shouldFinish = false;
    }

    for(int i = 0; i < base.size(); i++)
      if(base[i].name == "x0")
        return UNFEASIBLE;

    x.erase(x.end() - 1);
  }

  for(int i = 0; i < base.size(); i++)
    tempA[i].erase(tempA[i].end() - 1);

  tempC = c;

  double multipli;

  for(int i = 0; i < tempC.size(); i++){
    for(int j = 0; j < base.size(); j++){
      if(x[i].name == base[j].name){
        multipli = tempC[i];
        for(int z = 0; z < tempC.size(); z++){
          tempC[z] = tempC[z] + tempA[j][z] * multipli;
        }
      }
    }
  }

  if(verbose) printDict(tempC, tempA, base);

  //Our base is valid
  bool shouldFinish = true;
  bool unbound;
  double mostRestrictive = 0;
  for(int i = 0; i < tempC.size(); i++)
    if(tempC[i] > 0) shouldFinish = false;
  while(!shouldFinish){
    in = 0;
    for(int i = 0; i < tempC.size(); i++)
      if(tempC[in] < tempC[i]) in = i;

    out = 0;
    unbound = true;
    for(int i = 0; i < base.size(); i++){
      if(tempA[i][in] < -0.00001){
        if(unbound){
          unbound = false;
          mostRestrictive = -base[i].value / tempA[i][in];
          out = i;
        }
        else{
          if(-base[i].value / tempA[i][in] < mostRestrictive){
            mostRestrictive = -base[i].value / tempA[i][in];
            out = i;
          }
        }
      }
    }

    if(unbound)
      return UNBOUND;

    double mult;
    double lineDivisor = -tempA[out][in];
    for(int i = 0; i < x.size(); i++){
      tempA[out][i] = tempA[out][i] / lineDivisor;
    }
    base[out].name = x[in].name;
    base[out].value = base[out].value / lineDivisor;

    for(int i = 0; i < base.size(); i++){
      if(i == out) continue;
      mult = tempA[i][in];
      for(int j = 0; j < x.size(); j++)
        tempA[i][j] = tempA[i][j] + tempA[out][j] * mult;
      base[i].value = base[i].value + base[out].value * mult;
    }
    mult = tempC[in];
    for(int j = 0; j < x.size(); j++)
      tempC[j] = tempC[j] + tempA[out][j] * mult;

    if(verbose) printDict(tempC, tempA, base);

    shouldFinish = true;
    for(int i = 0; i < tempC.size(); i++)
      if(tempC[i] > 0) shouldFinish = false;
  }

  for(int i = 0; i < x.size(); i++)
    x[i].value = 0;
  for(int i = 0; i < base.size(); i++){
    for(int j = 0; j < x.size(); j++){
      if(x[j].name == base[i].name)
        x[j].value = base[i].value;
    }
  }

  for(int i = 0; i < b.size(); i++)
    removeVar("w" + std::to_string(i+1));

  this->optimal = true;
  return OPTIMAL;
}

void Program::printDict(Vet tempC, CoefMatrix tempA, VarVet base){
  printf("z = ");
  printLineMult(tempC, x);
  printf("\n--------------------------------------------------\n");
  for(int i = 0; i < base.size(); i++){
    printf("%s = %.2f ", base[i].name.c_str(), base[i].value);
    printLineMult(tempA[i], x);
    printf("\n");
  }
  printf("\n");
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

void Program::removeVar(std::string name){
  for(int i = 0; i < x.size(); i++){
    if(x[i].name == name){
      x.erase(x.begin() + i);
      c.erase(c.begin() + i);
      for(int j = 0; j < b.size(); j++)
        A[j].erase(A[j].begin() + i);
    }
  }
}
