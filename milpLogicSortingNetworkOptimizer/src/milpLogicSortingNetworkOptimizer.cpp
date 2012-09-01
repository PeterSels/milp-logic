#include <iostream>
#include <assert.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <vector>

using namespace std;

#include "GurobiSolver.h"
//#include "XpressSolver.h"
//#include "CplexSolver.h"

// User can turn on one of the below defines 
// by setting it to true to activate the respective test.

#ifdef USE_CPLEX_NATIVE
extern IloEnv * global_env_;
#endif

Solver * solver_;

void resetModel(unsigned int maxGetLicenseSeconds, 
                unsigned int maxSolverSeconds) {
  if (solver_==0) {
#ifdef USE_GUROBI_NATIVE
    solver_ = new GurobiSolver(maxGetLicenseSeconds, maxSolverSeconds);
#endif
#ifdef USE_XPRESS_NATIVE
    solver_ = new XpressSolver(maxGetLicenseSeconds, maxSolverSeconds);
#endif
#ifdef USE_CPLEX_NATIVE
    solver_ = new CplexSolver(maxGetLicenseSeconds, maxSolverSeconds);
#endif
  }
  assert(solver_!=0);
  solver_->resetModel();
}

char checkAndCalcChar(bool eqConn, bool andConn, bool orConn) {
  char c = '_';
  bool first  =  eqConn && !andConn && !orConn;
  bool second = !eqConn &&  andConn && !orConn;
  bool third  = !eqConn && !andConn &&  orConn;
  
  if (first) {
    c = '=';
  } else if (second) {
    c = '*';
  } else if (third) {
    c = '+';
  } else {
    assert(false);
  }
  return c;
}

int main(int argc, char * argv[]) {
    
  if (argc!=5) {
    cout << "Usage: " << argv[0] << endl
    << "  <nSortNetworkInputs(N)>" << endl
    << "  <nComparators(C)>" << endl
    << "  <minimizeComparators(1)IoFeasibility(0)>" << endl
    << "  <gap[0.0..1.0]>" << endl;
    return 0;
  }
  
  const int minN = 0;
  const int tempN = atoi(argv[1]);
  if (tempN < minN) {
    cout << "N cannot be less than " << minN << endl;
    cout << "Quitting." << endl;
    return 0;
  }
  const unsigned int N = (unsigned int)tempN;

  const long int minC = 0;
  const long int tempC = atoi(argv[2]);
  if (tempC < minC) {
    cout << "C cannot be less than " << minC << endl;
    cout << "Quitting." << endl;
    return 0;
  }
  const long unsigned int C = (unsigned long int)tempC;

  const int tempInt = atoi(argv[3]);
  if ((tempInt!=0) && (tempInt!=1)) {
    cout << "minimizeComparators(1)IoFeasibility(0) should be 0 or 1" << endl;
    cout << "Quitting." << endl;
    return 0;
  }
  const bool atMostIoExactlyOneComparatorPerColumn = (tempInt==1);

  const double gap = atof(argv[4]);
  if ((gap < 0.0) || (gap > 1.0)) {
    cout << "gap should be in [0.0 .. 1.0]" << endl;
    cout << "Quitting." << endl;
    return 0;
  }
  
  cout << "milpSolverOptimizer " << N << " " << C << " " 
  << atMostIoExactlyOneComparatorPerColumn << endl;
  
  cout << "N = " << N << endl;
  cout << "C = " << C << endl;
  cout << "atMostIoExactlyOneComparatorPerColumn = " 
  << atMostIoExactlyOneComparatorPerColumn << endl;
  cout << "gap = " << gap << endl;
  
  unsigned long int twoPowerN = (1 << N);
  
  cout << "Generating " << twoPowerN
  << " input output vectors ..." << endl;

  cout << "  unsorted input vectors" << endl;
  //bool b0[N][twoPowerN]; // MS Studio doesn't allow this, so:
  vector<vector<bool> > b0(N, vector<bool>(twoPowerN, false));
//#pragma omp parallel for num_threads(MAX_N_THREADS)
  for (unsigned long int x=0; x<twoPowerN; x++) {
    unsigned long int mask = (1 << (N-1));
    for (unsigned int y=0; y<N; y++) {
      b0[y][x] = ((x & mask)!=0);
      mask >>= 1;
    }
  }
  
  cout << "  sorted output vectors" << endl;
  //bool bC[N][twoPowerN]; // MS Studio doesn't allow this, so:
  vector<vector<bool> > bC(N, vector<bool>(twoPowerN, false));
//#pragma omp parallel for num_threads(MAX_N_THREADS)
  for (long int x=0; x<(long int)twoPowerN; x++) {
    unsigned int nOnes = 0;
    for (unsigned int y=0; y<N; y++) {
      nOnes += b0[y][x];
    }
    for (unsigned int y=0; y<N; y++) {
      bC[y][x] = ((N-1-y)<nOnes) ? 1 : 0;
    }
  }
  cout << "Generating " << twoPowerN 
  << " input output vectors done." << endl;

  unsigned int base10Width = (unsigned int)(log((double)twoPowerN)/log(10.0))+1;
  
  const bool explain = false;
  if (explain) {
    cout << "The number of comparators in the sorting network is choosen as C=" 
    << C << endl;
    cout << "twoPowerN = " << twoPowerN << endl;

    unsigned int base2Width = (unsigned int)(log((double)twoPowerN)/log(2.0));
    cout << "base10Width=" << base10Width 
    << ", base2Width=" << base2Width << endl;
    
    for (unsigned long int x=0; x<twoPowerN; x++) {
      cout << setw(base10Width) << x << " = ";
      for (unsigned int y=0; y<N; y++) {
        cout << b0[y][x];
      }
      cout << endl;
    }
    cout << endl;
    
    cout << "or in their transposed to the way sorting networks are drawn " << endl
    << "- sorting from left to right"  << endl
    << "- we choose: msb bits up and lsb bits down"  << endl;
    cout << "b0[0.." << N << "][0.." << twoPowerN << "] = " << endl;
    for (unsigned int y=0; y<N; y++) {
      for (unsigned long int x=0; x<twoPowerN; x++) {
        cout << b0[y][x];
      }
      cout << endl;
    }
    cout << "The " << twoPowerN 
    << " columns in here are the vectors that must be sorted." << endl;
    cout << "This is done by a series of comparators, from left to right." 
    << endl;    
    cout << "These are to be determined by this solver." << endl;

    cout << "We define sorting as all zeroes down, all ones up." << endl;
    cout << "So the sorted results must be:" << endl;

    cout << "in their transposed form:" << endl;
    cout << "bC=b" << C << "[0.." << N << "][0.." << twoPowerN << "] = " 
    << endl;
    for (unsigned int y=0; y<N; y++) {
      for (unsigned long int x=0; x<twoPowerN; x++) {
        cout << bC[y][x];
      }
      cout << endl;
    }

    cout << "Now for every column vector from b0, " << endl
    << "and associated column vector from bC, we enforce that the network " 
    << endl
    << "of comparators in between satisfies these ins and outs" << endl;
  }  
    
  solver_ = 0;
  resetModel(1*60, 5*60); // arguments used by cplex only
  solver_->setMinimize();

  const bool doUpdate = false;
  
  // generate all matrix VARIABLES    
  // iterate over network points, make

  cout << "Generating eqConnVar2D, andConnVar2D, orConnVar2D variables" << endl;
  vector<vector<SolverVar> > eqConnVar2D;
  vector<vector<SolverVar> > andConnVar2D;
  vector<vector<SolverVar> > orConnVar2D;
  vector<vector<vector<SolverVar> > > bVar3D;
  vector<vector<vector<SolverVar> > > eqConnImplVar3D;
  for (unsigned int y0=0; y0<N; y0++) {
    vector<SolverVar> eqConnVar1D;
    vector<SolverVar> andConnVar1D;
    vector<SolverVar> orConnVar1D;
    vector<vector<SolverVar> > bVar2D;
    vector<vector<SolverVar> > eqConnImplVar2D;
    for (unsigned long int x0=0; x0<=C; x0++) { // note <=C io <C
      // There are only C comparators, so
      if (x0 < C) { // skip x==C, for all but bVar3D
        stringstream strstr;
        strstr << y0 << "_" << x0;
        {
          string name = string("eqConnVar2D_") + strstr.str();
          const SolverVar eqConnVar = solver_->addBinVar(0, name, doUpdate);
          eqConnVar1D.push_back(eqConnVar);
        }
        {
          string name = string("andConnVar2D_") + strstr.str();
          const SolverVar andConnVar = solver_->addBinVar(0, name, doUpdate);
          andConnVar1D.push_back(andConnVar);
        }
        {
          string name = string("orConnVar2D_") + strstr.str();
          const SolverVar orConnVar = solver_->addBinVar(0, name, doUpdate);
          orConnVar1D.push_back(orConnVar);
        }
      }
      
      // but, in each row of the network, for each input vector,
      // we have (C+1) b variables from input to output, 
      // including input and output
      vector<SolverVar> bVar1D;
      vector<SolverVar> eqConnImplVar1D;
      // iterate over 2^N possible inputs and associated sorted outputs
      for (unsigned long int w0=0; w0<twoPowerN; w0++) {
        stringstream strstr;
        strstr << y0 << "_" << x0 << "_w0_" << "_" << w0;
        string suffix = strstr.str();
        {
          const int weight = 0; //(x0==C) ? (1<<(N-1-y0)) : 0; // pushes ones down
          string name = "bVar3D_" + suffix;
          const SolverVar bVar = solver_->addBinVar(weight, name, doUpdate);
          bVar1D.push_back(bVar);
        }
        {
          string name = string("eqConnImplVar3D_") + suffix;
          const SolverVar eqConnImplVar = solver_->addBinVar(0, name, doUpdate);
          eqConnImplVar1D.push_back(eqConnImplVar);
        }
      }
      bVar2D.push_back(bVar1D);
      eqConnImplVar2D.push_back(eqConnImplVar1D);
    }
    eqConnVar2D.push_back(eqConnVar1D);
    andConnVar2D.push_back(andConnVar1D);
    orConnVar2D.push_back(orConnVar1D);
    bVar3D.push_back(bVar2D);
    eqConnImplVar3D.push_back(eqConnImplVar2D);
  }
  solver_->update();
  
  cout << "Generating connChoice constraints" << endl;
  // Each position in the [y0][x0] matrix is either a =, *, or +
  for (unsigned int y0=0; y0<N; y0++) {
    for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C  
      stringstream strstr;
      strstr << y0 << "_" << x0;
      string suffix = strstr.str();
      SolverExpr connChoiceExpr
#ifdef USE_CPLEX_NATIVE
      (*global_env_)
#endif
      ;
      connChoiceExpr +=
      eqConnVar2D[y0][x0] + andConnVar2D[y0][x0] + orConnVar2D[y0][x0];
      string name = "connChoice_" + suffix;
      solver_->fastAddConstr(connChoiceExpr, "==", solver_->getOneExpr(), name);
    }
  }
  solver_->update();

  cout << "Generating "
  << "eqConnColSumExpr1D, eqConnColSumExpr1D, eqConnColSumExpr1D expressions" 
  << endl;
  // generate all matrix CONSTRAINTS
  // Each comparator has 2 in and outputs, so only 2 cVars per column are true
  vector<SolverExpr>  eqConnColSumExpr1D;
  vector<SolverExpr> andConnColSumExpr1D;
  vector<SolverExpr>  orConnColSumExpr1D;
  //vector<SolverExpr>    connColSumExpr1D;
  for (unsigned long int x0=0; x0<C; x0++) {
    SolverExpr eqConnColSumExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
    ;
    SolverExpr andConnColSumExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
    ;
    SolverExpr orConnColSumExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
    ;
    for (unsigned int y0=0; y0<N; y0++) {
       eqConnColSumExpr +=  eqConnVar2D[y0][x0];
      andConnColSumExpr += andConnVar2D[y0][x0];
       orConnColSumExpr +=  orConnVar2D[y0][x0];
    //     connColSumExpr +=    connVar2D[y0][x0];
    }
     eqConnColSumExpr1D.push_back( eqConnColSumExpr);
    andConnColSumExpr1D.push_back(andConnColSumExpr);
     orConnColSumExpr1D.push_back( orConnColSumExpr);
    //   connColSumExpr1D.push_back(   connColSumExpr);
  }
  solver_->update();
    
  cout << "Generating eqConnSumVsNMin2ForCol,"
  << " andConnSumVs1ForCol, orConnSumVs1ForCol," << endl
  << " andVsOrConnSumForCol,  allConnSumIsNForCol constraints" << endl;
  for (unsigned long int x0=0; x0<C; x0++) {
    stringstream strstr;
    strstr << x0;
    string suffix = strstr.str();
    {
      string name = "eqConnSumVsNMin2ForCol_" + suffix;
      string op = atMostIoExactlyOneComparatorPerColumn ? ">=" : "==";
      solver_->fastAddConstr(eqConnColSumExpr1D[x0], op, 
                             solver_->getOneExpr()*(N-2), name);
    }
    {
      string name = "andConnSumVs1ForCol_" + suffix;
      string op = atMostIoExactlyOneComparatorPerColumn ? "<=" : "==";
      solver_->fastAddConstr(andConnColSumExpr1D[x0], op, 
                             solver_->getOneExpr(), name);
    }
    {
      string name = "orConnSumVs1ForCol_" + suffix;
      string op = atMostIoExactlyOneComparatorPerColumn ? "<=" : "==";
      solver_->fastAddConstr(orConnColSumExpr1D[x0], op, 
                             solver_->getOneExpr(), name);
    }
    {
      string name = "andVsOrConnSumForCol_" + suffix;
      solver_->fastAddConstr(andConnColSumExpr1D[x0], "==",
                         orConnColSumExpr1D[x0], name);
    }
    {
      string name = "allConnSumIsNForCol_" + suffix;
      solver_->fastAddConstr(eqConnColSumExpr1D[x0] + 
                         andConnColSumExpr1D[x0] + 
                         orConnColSumExpr1D[x0], 
                         "==", solver_->getOneExpr()*N, name);
    }
    
  }
  solver_->update();
  
  cout << "Generating compConnVar3D variables" << endl;
  // for all x0
  // define comparator locations as variables with two y indices,
  // based on cVars with 1 y index
  // Note that there is no w0 index, because the comparator structure
  // should not depend on a specific input, since it is the same for all inputs.
  vector<vector<vector<SolverVar> > > compConnVar3D;
  for (unsigned int y0=0; y0<N; y0++) {
    vector<vector<SolverVar> > compConnVar2D;
    for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
      vector<SolverVar> compConnVar1D;
      for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
        stringstream strstr;
        strstr << y0 << "_gt_" << y1 << "_" << x0;
        string suffix = strstr.str();
        {
          string name = string("compConnVar3D_") + suffix;
          const SolverVar comp = solver_->addBinVar(1, name);
            // Note: cost 1 per comparator
          compConnVar1D.push_back(comp);
        }
      }
      compConnVar2D.push_back(compConnVar1D);
    }
    compConnVar3D.push_back(compConnVar2D);
  }  
  solver_->update();
  
  cout << "Generating compConnVar3DColSumIs1 constraints" << endl;
  // Allow only exactly one comparator per x0.
  // Later we can replace "==" with "<=" to also allow 0 comparators per x0.
  const string op = atMostIoExactlyOneComparatorPerColumn ? "<=" : "==";
  for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
    SolverExpr compSumExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
    ;
    for (unsigned int y0=0; y0<N; y0++) {
      for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
        compSumExpr += compConnVar3D[y0][y1][x0];
      }
    }
    stringstream strstr;
    strstr << x0;
    string suffix = strstr.str();
    {
      string name = string("compConnVar3DColSumIs1_") + suffix;
      solver_->fastAddConstr(compSumExpr, op, 
                             solver_->getOneExpr(), name);
        // operatorStr is == or <=
    }
  }
  solver_->update();
  
  cout << "Generating compConnImplAndConn, compConnImplOrConn " << endl
  << "  and compIsAndOrConnConj constraints" << endl;
  // truth for compConnVar3D implies truth for some [and|or]ConnVar2D
  for (unsigned int y0=0; y0<N; y0++) {
    for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
      for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
        stringstream strstr;
        strstr << y0 << "_gt_" << y1 << "_" << x0;
        string suffix = strstr.str();
        const bool usingOneWayImplication = false;
        if (usingOneWayImplication) {
          {
            string name = string("compConnImplAndConn_") + suffix;
            solver_->addImplication(compConnVar3D[y0][y1][x0], 
                                    andConnVar2D[y1][x0], name); // y1<y0
          }
          {
            string name = string("compConnImplOrConn_") + suffix;
            solver_->addImplication(compConnVar3D[y0][y1][x0], 
                                    orConnVar2D[y0][x0], name); // y0>y1
          }
        } else {
          {
            string name = string("compIsAndOrConnConj_") + suffix;
            solver_->addConjunctionConstr(compConnVar3D[y0][y1][x0],
                                          andConnVar2D[y1][x0], 
                                          orConnVar2D[y0][x0], 
                                          name);
          }
        }
      }
    }
  }
  solver_->update();
  
  // Prevent 'or' on lower y than 'and' 
  // (reduces combinatorial possibilitie by facto 2)
  
  // Is only really necessery iff atMostIoExactlyOneComparatorPerColumn.
  if (atMostIoExactlyOneComparatorPerColumn) { 
    // if false, seems faster without these andBeforeOr_ constraints
    // if true, these andBeforeOr_ constraints are definately needed
    
    cout << "Generating andBeforeOr constraints" << endl;
    // Can be done in many ways:
    // eg: sum sum comp == sum or = sum and, forall x0,
    // or: forall x0: forall y0: forall y1<y0 : and[y0] implies(==LE) (1-or[y1])
    // The last one seems more direct, so we choose that one.
    for (unsigned int y0=0; y0<N; y0++) {
      for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
        for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
          stringstream strstr;
          strstr << y0 << "_gt_" << y1 << "_" << x0;
          string suffix = strstr.str();
          {
            string name = string("andBeforeOr_") + suffix;
            solver_->fastAddConstr(andConnVar2D[y0][x0] + orConnVar2D[y1][x0], 
                               "<=", solver_->getOneExpr(), name); // y1<y0
          }
        }
      }
    }
    solver_->update();    
  }
  
  // Here we finish with bVar3D-independent constraints and start adding
  // constraints that involve these bVar3D. Consequently, 
  // these constraints below have an index [w0] in them.
  
  // For all x0,
  // define 'and' and 'or' values as variables with two y indices,
  // each based on 2 bVars with each only 1 y index.
  // Note that there is an w0 index, because the bVars also have it.  
  cout << "Generating andVar4D, orVar4D, andConnImplVar4D "
  << "and orConnImplVar4D variables" << endl;
  vector<vector<vector<vector<SolverVar> > > > andVar4D;
  vector<vector<vector<vector<SolverVar> > > > orVar4D;
  vector<vector<vector<vector<SolverVar> > > > andConnImplVar4D;
  vector<vector<vector<vector<SolverVar> > > > orConnImplVar4D;
  vector<vector<vector<vector<SolverVar> > > > andConnImplLeVar4D;
  vector<vector<vector<vector<SolverVar> > > > orConnImplLeVar4D;
  vector<vector<vector<vector<SolverVar> > > > andConnImplGeVar4D;
  vector<vector<vector<vector<SolverVar> > > > orConnImplGeVar4D;
  for (unsigned int y0=0; y0<N; y0++) {
    vector<vector<vector<SolverVar> > > andVar3D;
    vector<vector<vector<SolverVar> > > orVar3D;
    vector<vector<vector<SolverVar> > > andConnImplVar3D;
    vector<vector<vector<SolverVar> > > orConnImplVar3D;
    vector<vector<vector<SolverVar> > > andConnImplLeVar3D;
    vector<vector<vector<SolverVar> > > orConnImplLeVar3D;
    vector<vector<vector<SolverVar> > > andConnImplGeVar3D;
    vector<vector<vector<SolverVar> > > orConnImplGeVar3D;
    for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix        
      vector<vector<SolverVar> > andVar2D;
      vector<vector<SolverVar> > orVar2D;
      vector<vector<SolverVar> > andConnImplVar2D;
      vector<vector<SolverVar> > orConnImplVar2D;
      vector<vector<SolverVar> > andConnImplLeVar2D;
      vector<vector<SolverVar> > orConnImplLeVar2D;
      vector<vector<SolverVar> > andConnImplGeVar2D;
      vector<vector<SolverVar> > orConnImplGeVar2D;
      for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
        vector<SolverVar> andVar1D;
        vector<SolverVar> orVar1D;
        vector<SolverVar> andConnImplVar1D;
        vector<SolverVar> orConnImplVar1D;
        vector<SolverVar> andConnImplLeVar1D;
        vector<SolverVar> orConnImplLeVar1D;
        vector<SolverVar> andConnImplGeVar1D;
        vector<SolverVar> orConnImplGeVar1D;
        for (unsigned long int w0=0; w0<twoPowerN; w0++) {
          stringstream strstr;
          strstr << y0 << "_gt_" << y1 << "_" << x0 << "_w0_" << "_" << w0;
          string suffix = strstr.str();
          {
            string name = string("and4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            andVar1D.push_back(var);
          }
          {
            string name = string("or4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            orVar1D.push_back(var);
          }
          {
            string name = string("andConnImpl4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            andConnImplVar1D.push_back(var);
          }
          {
            string name = string("orConnImpl4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            orConnImplVar1D.push_back(var);
          }
          {
            string name = string("andConnImplLe4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            andConnImplLeVar1D.push_back(var);
          }
          {
            string name = string("orConnImplLe4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            orConnImplLeVar1D.push_back(var);
          }
          {
            string name = string("andConnImplGe4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            andConnImplGeVar1D.push_back(var);
          }
          {
            string name = string("orConnImplGe4D_") + suffix;
            const SolverVar var = solver_->addBinVar(0, name, doUpdate);
            orConnImplGeVar1D.push_back(var);
          }
        }
        andVar2D.push_back(andVar1D);
        orVar2D.push_back(orVar1D);        
        andConnImplVar2D.push_back(andConnImplVar1D);
        orConnImplVar2D.push_back(orConnImplVar1D);        
        andConnImplLeVar2D.push_back(andConnImplLeVar1D);
        orConnImplLeVar2D.push_back(orConnImplLeVar1D);        
        andConnImplGeVar2D.push_back(andConnImplGeVar1D);
        orConnImplGeVar2D.push_back(orConnImplGeVar1D);        
      }
      andVar3D.push_back(andVar2D);
      orVar3D.push_back(orVar2D);
      andConnImplVar3D.push_back(andConnImplVar2D);
      orConnImplVar3D.push_back(orConnImplVar2D);        
      andConnImplLeVar3D.push_back(andConnImplLeVar2D);
      orConnImplLeVar3D.push_back(orConnImplLeVar2D);
      andConnImplGeVar3D.push_back(andConnImplGeVar2D);
      orConnImplGeVar3D.push_back(orConnImplGeVar2D);        
    }
    andVar4D.push_back(andVar3D);
    orVar4D.push_back(orVar3D);
    andConnImplVar4D.push_back(andConnImplVar3D);
    orConnImplVar4D.push_back(orConnImplVar3D);        
    andConnImplLeVar4D.push_back(andConnImplLeVar3D);
    orConnImplLeVar4D.push_back(orConnImplLeVar3D);        
    andConnImplGeVar4D.push_back(andConnImplGeVar3D);
    orConnImplGeVar4D.push_back(orConnImplGeVar3D);        
  }
  solver_->update();

  
  const bool enforceEffectOnBVar3D = true;
  assert(enforceEffectOnBVar3D);
  if (enforceEffectOnBVar3D) {
    cout << "Generating (3D) eqConnImplDef, eqConnImplImpl" << endl
    << "  and (4D) andDef, orDef, andConnImplDef, andConnImplImpl, " << endl
    << "  orConnImplDef,  orConnImplImpl constraints" << endl;
    // Define the 'and' and 'or' variables as and resp. or of the correct bVars
    // This must hold for all possible 2^N inputs (and assoc. outputs)
    const unsigned long int w0PrintStep = 1;
    const unsigned int x0PrintStep = 1;
    const unsigned long nDots = twoPowerN * C;
    const unsigned long nConstraintsPerDot = 2 * N + 6 * (N * (N-1)/2);
    double nTotalConstraints = nDots * nConstraintsPerDot;
    cout << "  nDots = " << nDots << endl;
    cout << "  nConstraintsPerDot = " << nConstraintsPerDot << endl;
    cout << "  nTotalConstraints = " << nTotalConstraints << endl;
    cout << "  dot speed will decrease over time" << endl;
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      if (w0 % w0PrintStep == 0) {
        cerr << "    w0=" << setw(base10Width) << w0 
        << "/" << setw(base10Width) << twoPowerN << ", x0=./C=" << C << ": ";
      }
      for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
        if (x0 % x0PrintStep == 0) {
          cerr << ".";
        }
        for (unsigned int y0=0; y0<N; y0++) {
          stringstream strstr;
          strstr << y0 << "_" << x0 << "_w0_" << w0;
          string suffix = strstr.str();
          {
            string name = "eqConnImplDef_" + suffix;
            solver_->addEqualConstr(eqConnImplVar3D[y0][x0][w0],
                                    bVar3D[y0][x0+1][w0], 0, 1,
                                    bVar3D[y0][x0][w0], 0, 1,
                                    1, name);
          }
          
          {
            string name = "eqConnImplImpl_" + suffix;
            solver_->addImplication(eqConnVar2D[y0][x0], 
                                    eqConnImplVar3D[y0][x0][w0], name);
          }
          
          for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
            stringstream strstr;
            strstr << y0 << "_gt_" << y1 << "_" << x0 << "_w0_" << "_" << w0;
            string suffix = strstr.str();
            {
              {
                string name = "andDef_" + suffix;
                solver_->addConjunctionConstr(andVar4D[y0][y1][x0][w0],
                                              bVar3D[y0][x0][w0], 
                                              bVar3D[y1][x0][w0], 
                                              name);
              }
              {
                string name = "orDef_" + suffix;
                solver_->addDisjunctionConstr(orVar4D[y0][y1][x0][w0],
                                              bVar3D[y0][x0][w0], 
                                              bVar3D[y1][x0][w0], 
                                              name);
              }
            }
            
            {
              {
                string name = "andConnImplDef_" + suffix;
                solver_
                ->fastAddEqualConstr(andConnImplVar4D[y0][y1][x0][w0],
                                     andConnImplLeVar4D[y0][y1][x0][w0],
                                     andConnImplGeVar4D[y0][y1][x0][w0],
                                     bVar3D[y1][x0+1][w0], 0, 1, // y1<y0
                                     andVar4D[y0][y1][x0][w0], 0, 1,
                                     1, name);
              }
              {
                string name = "andConnImplImpl_" + suffix;
                solver_->addImplication(compConnVar3D[y0][y1][x0], 
                                        andConnImplVar4D[y0][y1][x0][w0], name);
              }
            }
            
            {
              {
                string name = "orConnImplDef_" + suffix;
                solver_
                ->fastAddEqualConstr(orConnImplVar4D[y0][y1][x0][w0],
                                     orConnImplLeVar4D[y0][y1][x0][w0],
                                     orConnImplGeVar4D[y0][y1][x0][w0],
                                     bVar3D[y0][x0+1][w0], 0, 1, // y0>y1
                                     orVar4D[y0][y1][x0][w0], 0, 1,
                                     1, name);
              }
              {
                string name = "orConnImplImpl_" + suffix;
                solver_->addImplication(compConnVar3D[y0][y1][x0], 
                                        orConnImplVar4D[y0][y1][x0][w0], name);
              }
            }
            
          }
        }
      }
      if (w0 % w0PrintStep == 0) {
        cerr << endl;
      }
    }
    solver_->update();
  }

  const bool enforceOutputConstraints = false;
  const bool enforceSortedOutputConstraints = true;
  assert(enforceOutputConstraints || enforceSortedOutputConstraints);
  cout << "Generating setInput and setOutput constraints" << endl;
  // connect first bVar3D x0-column to inputs,
  // and last b x0-column to its outputs
  const unsigned long int x0_0 = 0;
  const unsigned long int x0_C = C;
  for (unsigned int y0=0; y0<N; y0++) {
    // This must hold for all possible 2^N inputs (and assoc. outputs)
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {      
      {
        stringstream strstr;
        strstr << y0 << "_" << x0_0 << "_" << w0;
        string suffix = strstr.str();
        string name = string("setInput_") + suffix;
        solver_->fastAddConstr(bVar3D[y0][x0_0][w0], "==", 
                               solver_->getOneExpr() * (int)b0[y0][w0],
                               name);
      }
      
      {
        stringstream strstr;
        strstr << y0 << "_" << x0_C << "_" << w0;
        string suffix = strstr.str();
        if (enforceOutputConstraints) {
          string name = string("setOutput_") + suffix;
          solver_->fastAddConstr(bVar3D[y0][x0_C][w0], "==", 
                                 solver_->getOneExpr() * (int)bC[y0][w0],
                                 name);
        } 
        if (enforceSortedOutputConstraints) { 
          if (y0<N-1) {
            string name = string("sortedOutput_") + suffix;
            solver_->fastAddConstr(bVar3D[y0][x0_C][w0], "<=", 
                                   bVar3D[y0+1][x0_C][w0],
                                   name);
          }          
        }
      }
    }
  }
  solver_->update();
  
  
  if (atMostIoExactlyOneComparatorPerColumn) {
    // OPTIONAL:
    // Put all the possible all '=' columns in the front.
    // This is usefull to break symmetry for when in 
    // atMostIoExactlyOneComparatorPerColumn more with more columns than 
    // strictly necessary.
    const bool enforceGroupAndsOrsInFirstCols = true;
    cout << "enforceGroupAndsOrsInFirstCols = " 
    << enforceGroupAndsOrsInFirstCols << endl;
    if (enforceGroupAndsOrsInFirstCols) {
      cout << "Generating groupAndsInFirstCols and groupOrsInFirstCols "
      << "constraints" << endl;
      for (unsigned long int x0=0; x0<C-1; x0++) { // note <C-1 io <C !!
        SolverExpr andDiffSumExpr
#ifdef USE_CPLEX_NATIVE
        (*global_env_)
#endif
        ;
        SolverExpr orDiffSumExpr
#ifdef USE_CPLEX_NATIVE
        (*global_env_)
#endif
        ;
        stringstream strstr;
        strstr << x0;
        string suffix = strstr.str();
        for (unsigned int y0=0; y0<N; y0++) {
          // and
          andDiffSumExpr += andConnVar2D[y0][x0];
          andDiffSumExpr -= andConnVar2D[y0][x0+1];
          // or
          orDiffSumExpr += orConnVar2D[y0][x0];
          orDiffSumExpr -= orConnVar2D[y0][x0+1];
        }
        {
          string name = "groupAndsInFirstCols_" + suffix;
          solver_->fastAddConstr(andDiffSumExpr, ">=", 
                                 solver_->getNullExpr(), name);
        }
        {
          string name = "groupOrsInFirstCols_" + suffix;
          solver_->fastAddConstr(orDiffSumExpr, ">=", 
                                 solver_->getNullExpr(), name);
        }
      }
    }
    solver_->update();
  }
  
  // OPTIONAL:
  const bool enforceConstantColSumForBVarsForAllCols = false;
  cout << "enforceConstantColSumForBVarsForAllCols = " 
  << enforceConstantColSumForBVarsForAllCols << endl;
  if (enforceConstantColSumForBVarsForAllCols) {
    cout << "Generating bSumIsConstCols for all cols constraints" << endl;
    // for all input words (forall [w0]),
    // sum (over y0) of bits is the same for all [x0]-columns
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      for (unsigned long int x0=0; x0<C-1; x0++) { // note <C-1 io <C !!
        SolverExpr bDiffSumExpr
#ifdef USE_CPLEX_NATIVE
        (*global_env_)
#endif
        ;
        for (unsigned int y0=0; y0<N; y0++) {
          bDiffSumExpr += bVar3D[y0][x0][w0] - bVar3D[y0][x0+1][w0] ;        
        }
        stringstream strstr;
        strstr << x0 << "_w0_" << w0;
        string suffix = strstr.str();
        {
          string name = "bSumIsConstAllCols_" + suffix;
          solver_->fastAddConstr(bDiffSumExpr, "==", 
                             solver_->getNullExpr(), name);        
        }
      }
    }
    solver_->update();
  }
  
  
  // OPTIONAL:
  const bool enforceConstantColSumForBVarsForFirstAndLastCols = true;
  if (enforceConstantColSumForBVarsForFirstAndLastCols) {
    cout << "Generating bSumIsConstCols for first and last col constraints" 
    << endl;
    // for all input words (forall [w0]),
    // sum (over y0) of bits is the same for all [x0]-columns
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      unsigned int x0_0 = 0;
      unsigned int x0_C = C;
      SolverExpr bDiffSumExpr
#ifdef USE_CPLEX_NATIVE
      (*global_env_)
#endif
      ;
      for (unsigned int y0=0; y0<N; y0++) {
        bDiffSumExpr += bVar3D[y0][x0_0][w0] - bVar3D[y0][x0_C][w0] ;        
      }
      stringstream strstr;
      strstr << x0_0 << "_" << x0_C << "_w0_" << w0;
      string suffix = strstr.str();
      {
        string name = "bSumIsConst2Cols_" + suffix;
        solver_->fastAddConstr(bDiffSumExpr, "==", 
                           solver_->getNullExpr(), name);        
      }
    }
    solver_->update();
  }
  
  
  
  
  // OPTIONAL:
  // Enforce that for each [w0] word, 
  // the 2 outputs of each comparator are sorted.
  // This can be done independent of their inputs.
  const bool enforceCompOutputBitsSorted = true;
  cout << "enforceCompOutputBitsSorted = " 
  << enforceCompOutputBitsSorted << endl;
  if (enforceCompOutputBitsSorted) {
    cout << "Generating compOutputBitsSorted constraints" << endl;
    const long unsigned int w0PrintStep = 4;
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      if (w0 % w0PrintStep == 0) {
        cerr << "    w0=" << setw(base10Width) << w0 
        << "/" << setw(base10Width) << twoPowerN;
      }
      for (unsigned int y0=0; y0<N; y0++) {
        for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
          for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
            stringstream strstr;
            strstr << y0 << "_gt_" << y1 << "_" << x0 << "_w0_" << w0;
            string suffix = strstr.str();
            {
              string name = "compOutputBitsSorted_" + suffix;
              // We want to enforce 'a implies (b LE c)'
              // This is equiv. with 'a + b LE 2c + 1' (by checked truth table).
              solver_->fastAddConstr(compConnVar3D[y0][y1][x0] + 
                                 bVar3D[y1][x0+1][w0], // y1!
                                 "<=", 
                                 2 * bVar3D[y0][x0+1][w0] + 1, name); // y0!
              // This avoids having to generate a new solver var for (b LE c).
            }
          }
        }
      }
      if (w0 % w0PrintStep == 0) {
        cerr << endl;
      }
    }
    solver_->update();
  }
  
  
  // write  out model files for debugging
  const bool writeOutModelFiles = false;
  if (writeOutModelFiles) {
    string fileNamePrefix = "SortOptimizer";
    cout << "Writing out model fileNamePrefix.(lp,mps,rew) files" << endl; 
    solver_->exportModelAsLpFile(fileNamePrefix);
    solver_->exportModelAsMpsFile(fileNamePrefix);
    solver_->exportModelAsRewFile(fileNamePrefix);
  }
  
  cout << "Size of model" << endl;
  unsigned int nRows = solver_->getNumberOfRows();
  unsigned int nCols = solver_->getNumberOfColumns();
  unsigned int nNonZeroes = solver_->getNumberOfNonZeroes();
  cout << "(r,c,nz) = (" << nRows << ", " << nCols << ", " 
  << nNonZeroes << ")" << endl;
  unsigned int nSets = solver_->getNumberOfSets();
  cout << "ns = " << nSets << endl;

  
  // solve
  cout << "Solving model downto gap " << gap*100 << "%." << endl;
  double twoDaysTimeInSeconds = 2 * 24 * 60 * 60;
  bool solved = solver_->timedSolve(gap, 4, twoDaysTimeInSeconds);
  
  // check solver status
  if (!solved) {
    cout << "Optimization problem could not be solved." << endl;
    exit(0);
  }
  
  assert(solved);
  double objVal = solver_->getObjVal();
  cout << "goal function value = " << objVal << endl;   
  
  // supposing it's solved,
  // read back result
  //char c[N][C]; // MS Studio doesn't allow this, so:
  vector<vector<char> > c(N, vector<char>(C, '_'));
  cout << "Optimal comparator sorting network =" << endl;
  for (unsigned int y0=0; y0<N; y0++) {
    for (unsigned long int x0=0; x0<C; x0++) {
      bool  eqConn = (solver_->getValueOf( eqConnVar2D[y0][x0])!=0);
      bool andConn = (solver_->getValueOf(andConnVar2D[y0][x0])!=0);
      bool  orConn = (solver_->getValueOf( orConnVar2D[y0][x0])!=0);
      char ch = checkAndCalcChar(eqConn, andConn, orConn);
      c[y0][x0] = ch;
      cout << ch;
    }
    cout << endl;
  }
  cout << endl;


  cout << "Checking result with all " << twoPowerN << " in- and output vectors" 
  << endl;
  const bool check = true;
  bool ok = true;
  if (check) {
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      //bool b[N][C+1]; // MS Studio doesn't allow this, so:
      vector<vector<bool> > b(N, vector<bool>(C+1, false));
      // initialize first column (x0=0)
      for (unsigned int y0=0; y0<N; y0++) {
        b[y0][0] = b0[y0][w0];
      }
      // propagate columns
      for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
        unsigned int andIndex = N; // illegal value
        unsigned int orIndex = N; // idem
        unsigned int nEquals = 0;
        for (unsigned int y0=0; y0<N; y0++) {
          if (c[y0][x0]=='=') {
            b[y0][x0+1] = b[y0][x0];
            nEquals++;
          } else if (c[y0][x0]=='*') {
            andIndex = y0;
          } else if (c[y0][x0]=='+') {
            orIndex = y0;
          }
        }
        if (andIndex==N) {
          assert(orIndex==N);
          assert(nEquals==N);
        } else {
          assert(andIndex>=0);
          assert(orIndex>=0);
          assert(andIndex<N);
          assert(orIndex<N);
          assert(nEquals==N-2);
          b[andIndex][x0+1] = b[andIndex][x0] && b[ orIndex][x0];
          b[ orIndex][x0+1] = b[andIndex][x0] || b[ orIndex][x0];          
        }
      }
      // check last column (x0=C)
      for (unsigned int y0=0; y0<N; y0++) {
        if (b[y0][C] != bC[y0][w0]) {
          ok = false;
          cerr << "ERROR: b[y0][0] != bC[y0][w0]" << endl;
          cerr << "       b[" << y0 << "][0] != bC[" << y0 << "][" << w0 << "]" 
          << endl;
        }
      }      
    }
  }
  if (ok) {
    cout << "Check is OK." << endl;
  } else {
    cout << "Check is NOK." << endl;    
  }
  
  bool debugging = false;
  if (debugging) {
    cout << "again cVar2D[y0][x0] but also calling " << endl;
  
    // Show how for all w0 it sorts the bits:
    cout << "b0[y0][x0] bVar3D[y0][x0][w0] bc[y0][x0]" << endl;
    for (unsigned long int w0=0; w0<twoPowerN; w0++) {
      cout << "(w0=" << w0 << ")" << endl;
      
      for (unsigned int y0=0; y0<N; y0++) {
        for (unsigned long int x0=0; x0<=C; x0++) { // note <=C io <C
          
          // fixed input
          if (x0==0) {
            cout << b0[y0][w0] << "=";
          }
          
          // variable part
          bool bVar = (solver_->getValueOf(bVar3D[y0][x0][w0])!=0);
          cout << (bVar ? '1' : '0');
          
          if (x0<C) {
            char ch = c[y0][x0];
            cout << ch;          
          }
          
          // fixed output
          if (x0==C) {
            cout << "=" << bC[y0][w0];
          }
        }
        cout << endl;
      }
      cout << endl;
    }
    
    cout << "compConnVar3D[y0][y1][x0]" << endl;
    for (unsigned int y0=0; y0<N; y0++) {
      for (unsigned int y1=0; y1<y0; y1++) { // lower triangle of matrix
        cout << "(y0,y1)=(" << y0 << "," << y1 << "): ";
        for (unsigned long int x0=0; x0<C; x0++) { // note <C io <=C
          bool compConnVar = 
		    (solver_->getValueOf(compConnVar3D[y0][y1][x0])!=0);
          cout << (compConnVar ? '1' : '0');
        }
        cout << endl;
      }
    }
    cout << endl;
  }
  
  return 0;
}
