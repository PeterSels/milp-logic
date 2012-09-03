This the file README.txt for the project 'MilpLogic'.

What is this?
-----------

MilpLogic is a C++ MILP Solver Abstraction Layer with a C++ Boolean Modelling Layer on Top.
MILP stands for Mixed Integer Linear Programming.
More on MILP can be found on
http://en.wikipedia.org/wiki/Mixed_integer_linear_programming#Integer_unknowns


What are the Project Goals?
-------------------------

It's intended to be used by C++ programmers as a library to make:

(1) the setting up of general MILP (Mixed Integer Linear Programming)
models more *solver independent*
This is done by providing a solver independent general MILP building API in the class 'Solver'.

(2) the setting up of MILP models with lots of *boolean variables and
their logical constraints( (like implication, negation, conjunction,
disjunction) *a lot easier*.
This is done by providing a (again solver independent) boolean logic
variable and constraint addition API in the class 'Solver'.
Internally and automatically the implied linear equations and
inequalities equivalent with the wanted boolean relations are generated.

(3) the resulting C++ code generating MILP model with this MilpLogic API impiore readable.

(4) If you want, you can also see the scalability tests in milpLogicSortingNetworkOptimizer
as a solver benchmark. For these milpLogicSortingNetworkOptimizer
models, some solvers solve these models in significantly lower time than others ...



How to Search MilpLogic?
-----------------------

You did already. If not, google for 'MilpLogic'. You'll find it, since the project is hosted with them at http://code.google.com



How to Download?
-----------------

Go to http://code.google.com/p/milp-logic/

Checkout sources.



Directory structure
-----------------

The projects' structure, when you just downloaded it, should look like this:

$ ./showDirTree.sh
+----MilpLogicClone
     +----CMakeLists.txt
     +----README.txt
     +----boost.cmake
     +----doc
     |    +----Doxyfile
     |    +----run.sh
     +----libMilpLogic
     |    +----CMakeLists.txt
     |    +----include
     |    |    +----BclException.h
     |    |    +----CplexExpression.h
     |    |    +----CplexSolver.h
     |    |    +----GurobiSolver.h
     |    |    +----HasSolver.h
     |    |    +----Solver.h
     |    |    +----XpressSolver.h
     |    +----src
     |         +----BclException.cpp
     |         +----CplexExpression.cpp
     |         +----CplexSolver.cpp
     |         +----GurobiSolver.cpp
     |         +----Solver.cpp
     |         +----XpressSolver.cpp
     +----milpLogicSortingNetworkOptimizer
     |    +----CMakeLists.txt
     |    +----run.sh
     |    +----src
     |         +----milpLogicSortingNetworkOptimizer.cpp
     +----milpLogicTest
     |    +----CMakeLists.txt
     |    +----run.sh
     |    +----src
     |         +----milpLogicTest.cpp
     +----regenerateProjectFiles.sh
     +----showDirTree.bat
     +----showDirTree.sh
     +----tree.pl
     +----win_tree.pl
$

To see your structure, from the command line, run 

./showDirTree.sh 

on *nix systems, and 

showDirTree.bat

on Windows systems.

     +----doc
contains the projects'documentation.
It also contains a the doxygen settings file 'Doxygen'.
To generate doxygen documentation, just go to the doc directory and type:

./run.sh

This will generate a latex and html directory containing extensive documentation on this project.
This includes class relationship diagrams, calling graphs, called-by graphs and more.

     +----libMilpLogic
contains the library for solver abstraction as well as the layer for making MILP models with boolean variable solver variables.

     +----milpLogicTest
contains as simple as possible tests for some API calls from libMilpLogic. It is intended as documentation about how to use the API.

     +----milpLogicSortingNetworkOptimizer
contains a much more involved example, illustrating the scalability of the libMilpLogic library and its APIs.
It also illustrates how to write code for both solvers that don't use an update() call between MILP variable generation (Cplex, Xpress)
and solvers that do (Gurobi). Since the number of contraints for this MILP model grow exponentially with
the input parameters to the milpLogicSortingNetworkOptimizer executable, this also serves as a benchmark for 
MILP models of containing almost only boolean solver variables accross the different solvers one tries.


Setting Environment Variables MilpLogic depends on
------------------------------------------------

Before you should do any builds, first set these environment
variables. This holds for *nix OSes as well as for Windows versions.

For Windows version, you have to manualle edit the environment
variables in Control Panel > All Control Panel Items > Advanced system
settings > (Confirm Yes) >  

Select the 'Advance' tab if not selected already.

Click Environment Variables...

Edit the System Variables, the lowest section of the two.
This has the advantage that all users will see the changes you apply
instead of only you.

For example, in the case you are usng the solver Gurobi v5.0.1,
set GUROBI_HOME to C:\gurobi501\win64
or wherever it is installed.

Set GUROBI_MAIN_VERSION to GUROBI50.

For CPLEX, for example, if you are using v12.3,
set CPLEX_HOME to C:\IBM\ILOG\CPLEX_Studio123\cplex
set CONCERT_HOME to  C:\IBM\ILOG\CPLEX_Studio123\concert
set CPLEX_LIB to cplex123
set CPLEX_VERSION to 123

For Xpress
set XPRESS_HOME to C:\XPRESSMP

Supported version of XPRESS, is for now, Xpress from Fico v7.2.1

On *nix systems, you can add the following lines to your  ~/.bashrc file
or  ~/.profile file. 

As an example, the relevant section in my ~/.profile file is:

export CPLEX_HOME=/opt/ibm/ILOG/CPLEX_Studio_Academic124/cplex
export CONCERT_HOME=/opt/ibm/ILOG/CPLEX_Studio_Academic124/concert
export CPLEX_ARCH_DIR=x86-64_darwin9_gcc4.0

export ILOG_LICENSE_FILE=/usr/ilog/ilm/access.ilm
export CPLEX_VERSION=

#export GUROBI_MAIN_VERSION=gurobi30
#export GUROBI_MAIN_VERSION=gurobi40
#export GUROBI_MAIN_VERSION=gurobi45
#export GUROBI_MAIN_VERSION=gurobi46
export GUROBI_MAIN_VERSION=gurobi50

#export GUROBI_VERSION=gurobi301
#export GUROBI_VERSION=gurobi400
#export GUROBI_VERSION=gurobi401
#export GUROBI_VERSION=gurobi450
#export GUROBI_VERSION=gurobi451
#export GUROBI_VERSION=gurobi452
#export GUROBI_VERSION=gurobi460
#export GUROBI_VERSION=gurobi461
#export GUROBI_VERSION=gurobi500
export GUROBI_VERSION=gurobi501

export GUROBI_HOME="/Library/${GUROBI_VERSION}/mac64"
export PATH="${PATH}:${GUROBI_HOME}/bin"
#export DYLD_LIBRARY_PATH="${GUROBI_HOME}/lib"
export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${GUROBI_HOME}/lib"

#export GRB_LICENSE_FILE="${GUROBI_HOME}/gurobi.lic" 
# The above line is only need if not put on this standard location

export MILP_LOGIC_HOME=~peter/projects/MilpLogic


After adding the correct environment variables, don't forget to activate these settings by
sourcing the changed file. For bash you do this by typing in your terminal:

. ~peter/.profile

for tsh you do this by typing

source ~peter/.profile



What does MilpLogic Depend On?
------------------------------

This software has been built, installed and run on:
- Apple OS X 10.8.1
- Linux Ubuntu
- Windows XP
- Windows 7

All code is C++ code. It also includes some bash files, which are recognizable by their .sh extension
and Windows batch files recognizable by their .bat extension.

It is set up generically, using CMake for Makefile and/or project generation.

The project depends on:
- C and C++ compiler (free, tested with  v4.2.1)
- STL free, (included in c++ compiler)
- Boost library (free, use v1.50.0 or later) (only for the timer in Solver::timedSolve())
- CMake (free, use v2.6 or later)

To be able to build and run, you will also need at least one of the MILP solvers that MilpSolver currently abstracts.
These are currently: CPLEX, Gurobi, Fico XPress. You can extend MipLogic for other servers too of course.
Solver selection is done prior to building, by changing the defines in the header file libMilpLogic/HasSolver.h.

To select Cplex, activate the line:
#define USE_CPLEX_NATIVE

To select Gurobi, activate the line:
#define USE_GUROBI_NATIVE

To select Xpress activate the line:
#define USE_XPRESS_NATIVE

Currently, only one of these defines can be active at any time.

These three solvers have been tested up to the versions:
Cplex 12.4, Gurobi v5.0.1, Fico Xpress 7.2.
Slightly older versions also have been tested and work.
For newer versions, there is no guarantee, but usually little or nothing is changed in the solver APIs
and solver upgrades mainly improve performance of the internal algorithms.

You will also need a license for any of the three mentioned solvers.


What is the MilpLogic License Model?
----------------------------------

MilpLogic is open source software, subject to a GPL v3 license.
See http://www.gnu.org/copyleft/gpl.html
for the complete license.


How to Build and Install MilpLogic, including Examples?
---------------------------------------------------

Via CMake, MilpLogic can setup project files for:
- Microsoft Windows Visual Studio C++ sln and prj files (Download and use the CMake GUI for Windows)
- plain Unix Makefiles, to be used at command line, typing just 'make' (just type ./regenerateProjectFiles.sh)
- Apples XCode IDE  (just type ./regenerateProjectFiles.sh xcode)
- Eclipse  (just type ./regenerateProjectFiles.sh eclipse)

First set the environment variable MILP_LOGIC_HOME to the full path up
to MilpLogic. (eg: /Users/peter/projects/MilpLogic).

So run cmake on the file MilpLogic/CmakeLists.txt.

For Windows you do this with the CMake GUI, which can be downloaded
for free from http://www.cmake.org.
Select the project: MilpLogic and click the Configure and then Setup
buttons. You know generated a project called MilpLogi.sln in the
MilpSolver directory. Load it in MicroSoft Visual Studio C++ and build
it, run it, zip it, ship it, ...

For *nix systems, in a terminal, simply type

./regenerateProjectFiles.sh xcode

to generate an XCode project, called MilpLogic.xcodeproject in the MilpLogic directory

or

./regenerateProjectFiles.sh eclipse

to generate an Eclipse project, called MilpLogic.project in the MilpLogic directory

or 

either 

./regenerateProjectFiles.sh

or 

./regenerateProjectFiles.sh unixmakefiles

to generate Makefiles that can be used by typing

make 

in the MilpLogic directory, without the need for an IDE.



How to Run the Tests?
--------------------

There are some simple tests and a more involved one.

The simple tests are in the directory

     +----milpLogicTest

Just go to this directory and in the terminal, type:

./run.sh

In the C++ code in the file milpLogicTest/src/milpLogicTest.cpp,
you can select which test you want to run by changing some defines at the beginning of the file.

The more involved test is in the directory

     +----milpLogicSortingNetworkOptimizer

Just go to this directory and in the terminal, type:
./milpLogicSortingNetworkOptimizer 


Where is the documentation?
--------------------------

We consider the test in
     +----milpLogicTest
     +----milpLogicSortingNetworkOptimizer
as executable - and hence, always up to date - documentation.

You can also generate documentation with doxygen in the 
     +----doc
subdirectory of MilpLogic.

On *nix systems, just go there, and type 

doxygen

or on Windows systems, use the doxygen GUI.

doxygen and its GUI can be downloaded freely from http://www.doxygen.org.

doxygen generates class inheritance graphs, function call graphs.
It also generates html interlinked code in the MilpLogic/doc/html subdirectory and latex and pdf files in the
MilpLogic/doc/latex subdirectory.


Can this project be Improved?
---------------------------

Yes, sure.


How can this project be Improved?
-------------------------------

Since this is an open source project, anyonre can contribute to this project.
Some ideas on improvements are mentioned in the Feature Wish List below.
You may have others.


Feature Wish List
----------------

- Currently, when using the Cplex solver (by defining USE_CPLEX_NATIVE in HasSolver.h), the lpmodel written out does not write out the goal function.
it does write out all constraints though. For Xpress and Gurobi, both goal function and all constraints are writen out. It is probably only a small fix.

- Currently, the defines in HasSolver.h determine for which solver the MilpLogic library will be built. As a consequence only one solver can be used at a time.
When changing solvers, one can change a define in the HasSolver.h header file and rebuild. THis is currently only seen as a minor disadvantage.

- For some API calls, for some solvers, an implementation still lacks. assert(false) and exit(0) are present there. It would be better to have an implementation 
there for all solvers. Lack of time is the reason there isn't one yet everywhere. This is one of the reasons for open sourcing this projects' code.

- Gurobi allows starting values, per separate solver variable in an easy way. The API call Solver::setStartValue(SolverVar) is the solver agnostic call for this.
In Cplex and Xpress, this call is not implemented yet, since the exact same functionality as in Gurobi is absent.
Cplex and Xpress only allow starting values in the form of a full matrix of them, for all solver variables.
One idea for implementing Solver::setStartValue(SolverVar)  then, for both Cplex and Xpress would be to cache the Solver::setStartValue(SolverVar)
and store anc cache the starting values in a matrix, that is only read out later as a whole matrix, when actually starting the solver.

- When building for xcode, we currnetly get some warnings:
ld: warning: directory not found for option '-L/opt/ibm/ILOG/CPLEX_Studio_Academic124/cplex/lib/x86-64_darwin9_gcc4.0/static_pic/Debug'
ld: warning: directory not found for option '-L/opt/ibm/ILOG/CPLEX_Studio_Academic124/concert/lib/x86-64_darwin9_gcc4.0/static_pic/Debug'
ld: warning: directory not found for option '-L/Library/gurobi501/mac64/lib/Debug'
ld: warning: directory not found for option '-L/usr/local/boost_1_50_0/stage/lib/Debug'
This is because Debug is somehow appended to the specified library directories in the CMakeLists.txt file.
The directory where the libraries are specified and found by the linker are the ones without the /Debug suffix, so building does go well,
but we would like to get rid of these warnings.

 - Give wanring about time limit exceedin solver time limit. This works for Gurobi, but is still to implement for Cplex and Xpress.


How to Contribute to this Project?
-------------------------------

Go to http://code.google.com and search for 'MilpLogic'.
Search for MilpLogic.

Git clone sources.
Change sources.
Git push changed sources.

Read more on it on http://code.google.com.



How to use MilpLogic API in my own C++ Program?
-----------------------------------------------

In HasSolver.h, activate the define you want, for example:

#define USE_GUROBI_NATIVE

Then in your own code include the corresponding solver include file

#include "GurobiSolver.h"

For Cplex only, add

#ifdef USE_CPLEX_NATIVE
extern IloEnv * global_env_;
#endif

and in any case, add one global variable, called solver_.

Solver * solver_;

To have generic resetModel function, you can add this:

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

Starting a model is now as simple as:

  solver_ = 0;
  resetModel(1*60, 5*60); // arguments currently used by cplex only
  solver_->setMinimize(); // or solver_->setMaximize();

  const bool doUpdate = false;

The doUpdate is a boolean that is used by Gurobi to update its state
after adding variables. If set to true, you can define constraints
over the variables only after you updated the model.
This is designed so for efficiency reasons.

Then one can start adding boolean variables:

          string name0 = "a";
          const SolverVar aVar = solver_->addBinVar(0, name, doUpdate);

          string name0 = "b";
          const SolverVar bVar = solver_->addBinVar(0, name, doUpdate);

          solver_->update();

and now constraints:

          string constrName = "aEQb";
          solver_->fastAddConstr(aVar, "==", bVar, constrName);

More involved examples are present in the directories: milpLogicTest
and milpLogicSortingNetworkOptimizer.



Questions?
----------

sels
dot
peter 
at
gmail
dot 
com
