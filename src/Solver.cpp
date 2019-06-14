#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "../include/Program.h"

Program readProgram(char fileName[]){
  VarVet x;
  Var temp;
  Vet c;
  Vet b;

  CoefMatrix A;
  Vet tempVet;

  char arq[20000];
  char *finder;

  std::string collector;

  int numConstraints;

  int i = 0;
  FILE *fp = fopen(fileName, "r");
  while(!feof(fp)){
    arq[i] = fgetc(fp);
    i++;
  }
  arq[i] = '\0';
  fclose(fp);

  finder = strstr(arq, "#Weights");
  while(*finder != '\n')
    finder++;
  finder++;

  while(*finder != '#'){
    collector.clear();
    while(*finder != '\n'){
      collector.push_back(*finder);
      finder++;
    }

    c.push_back(std::stod(collector));
    finder++;
  }

  while(*finder != '\n')
    finder++;
  finder++;

  while(*finder != '#'){
    collector.clear();
    while(*finder != '\n'){
      collector.push_back(*finder);
      finder++;
    }

    temp.name = collector;
    temp.real = true;
    x.push_back(temp);

    finder++;
  }

  while(*finder != '\n')
    finder++;
  finder++;

  collector.clear();
  while(*finder != '\n'){
    collector.push_back(*finder);
    finder++;
  }
  finder++;

  numConstraints = std::stoi(collector);
  for(int i = 0; i < numConstraints; i++){
    tempVet.clear();
    for(int j = 0; j < x.size(); j++){
      collector.clear();
      while(*finder != '\n'){
        collector.push_back(*finder);
        finder++;
      }
      finder++;

      tempVet.push_back(std::stod(collector));
    }
    A.push_back(tempVet);

    collector.clear();
    while(*finder != '\n'){
      collector.push_back(*finder);
      finder++;
    }
    finder++;

    b.push_back(std::stod(collector));
  }

  Program result(c, b, x, A);

  return result;
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("Usage: %s PROGRAM\n", argv[0]);
    return 0;
  }

  bool verbose = false;
  if(argc > 2 && !strcmp(argv[2], "-v"))
    verbose = true;

  Program p = readProgram(argv[1]);

  p.printProgram();
  printf("\n");

  switch (p.optimize(verbose)) {
    case OPTIMAL:
      printf("Program is optimal with objective value: %.2f", p.getObjectiveValue());
      printf("\nVars:\n");
      for(auto var : p.getVars()){
        printf("%s: %.2f\n", var.name.c_str(), var.value);
      }
      break;
    case UNBOUND:
      printf("Program is unbound");
      break;
    case UNFEASIBLE:
      printf("Program is unfeasible");
      break;
  }

  return 0;
}
