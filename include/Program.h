#include <string>
#include <vector>

typedef std::vector<double> Vet;

typedef struct vars{
    double value;
    std::string name;
    bool real;
} Var;

typedef std::vector<Var> VarVet;

typedef std::vector<Vet> CoefMatrix;

class Program{
  public:
    Program(Vet c, Vet b, VarVet x, CoefMatrix A);

    void printProgram();

    void optimize();

    bool isValid(){return valid;}
    bool isOptimal(){return optimal;}

  private:
    Vet c;
    Vet b;
    VarVet x;
    CoefMatrix A;
    bool valid, optimal;

    double dAbs(double d);

    void printLineMult(Vet v, VarVet x);

    void verifyValidForm();
};
