#include <string>
#include <vector>

enum SolverResult {OPTIMAL, UNBOUND, UNFEASIBLE, INVALID};

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

    double getObjectiveValue();
    VarVet getVars();

    SolverResult optimize(bool verbose);

    bool isValid(){return valid;}
    bool isOptimal(){return optimal;}

  private:
    Vet c;
    Vet b;
    VarVet x;
    CoefMatrix A;
    bool valid, optimal;

    SolverResult simplex(bool verbose);
    void printDict(Vet tempC, CoefMatrix tempA, VarVet base);

    double dAbs(double d);

    void printLineMult(Vet v, VarVet x);

    void verifyValidForm();

    void removeVar(std::string name);
};
