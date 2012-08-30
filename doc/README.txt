This the file README.txt for the project 'MilpLogic'.

What is this?
-----------

MilpLogic is A Solver Abstraction Layer with a Boolean Layer on Top,
all written in C++.


Project Goals?
-------------

It's intended to be used by C++ programmers as a library to make:

(1) the setting up of general MILP (Mixed Integer Linear Programming)
models more *solver independent*
This is done by providing a solver independent general MILP building API in the class 'Solver'.

(2) the setting up of MILP models with lots of *boolean variables and
their logical constraints( (like implciation, negation, conjunction,
disjunction) *a lot easier*.
This is done by providing a (again solver independent) boolean logic
variable and constraint addition API in the class 'Solver'.
Internally and automatically the implied linear equations and
inequalities equivalent with the wanted boolean relations are generated.

(3) the resulting C++ code generating MILP model with this MilpLogic API more readable.


How to Search MilpLogic?
-----------------------

You did already. If not, google for 'MilpLogic'. You'll find it, since the project is hosted with them at http://code.google.com



How to Download?
-----------------

Go to http://code.google.com and search for 'MilpLogic'.
Search for MilpLogic.
Checkout sousces.

Directory structure
-----------------

The projects' structure, when you just downloaded it, should look like this:

$ ./showDirTree.sh
+----MilpLogic
     +----CMakeLists.txt
     +----boost.cmake
     +----doc
     |    +----Doxyfile
     |    +----README.txt
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
     |    +----src
     |         +----milpLogicSortingNetworkOptimizer.cpp
     +----milpLogicTest
     |    +----CMakeLists.txt
     |    +----src
     |         +----milpLogicTest.cpp
     +----redoCmx.sh
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
contains the projects'documentation. It contains this README.txt file.
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


How to Build and Install?
-----------------------

This software has been built, installed and run on:
- Apple OS X 10.8.1
- Linux Ubuntu
- Windows XP
- Windows 7

All code is C++ code. It also includes some bash files, which are recognizable by their .sh extension
and Windows batch files recognizable by their .bat extension.

It is set up generically, using CMake for Makefile and/or project generation.

The project depands on:
- C and C++ compiler (free, tested with  v4.2.1)
- STL free, (included in c++ compiler)
- Boost library (free, use v1.50.0 or later)
- CMake (free, use v2.6 or later)

To be able to build and run, you will need at least one of the MILP solvers that MilpSolver currently abstracts.
These are currently: CPLEX, Gurobi, Fico XPress. You can extend MipLogic for other servers too of course.
Solver selection is done prior to building, by changing the defines in the header file libMilpLogic/HasSolver.h.

To select Cplex, activate:
#define USE_CPLEX_NATIVE

To select Gurobi, activate:
define USE_GUROBI_NATIVE

To define Xpressm activate:
#define USE_XPRESS_NATIVE

Currently, only one of these defines can be active at any time.

These three solvers have been tested up to the versions:
Cplex 12.4, Gurobi v5.0.1, Fico Xpress 7.2.
Slightly older versions also have been tested and work.
For newer versions, there is no guarantee, but usually little or nothing is changed in the solver APIs
and solver upgrades mainly improve performance of the internal algorithms.


Via CMake, it can setup project files for:
- Microsoft Windows Visual Studio C++ sln and prj files (Download and use the CMake GUI for Windows)
- plain Unix Makefiles, to be used at command line, typing just 'make' (just type ./redoCmx.sh)
- Apples XCode IDE  (just type ./redoCmx.sh xcode)
- Eclipse  (just type ./redoCmx.sh eclipse)



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

 

How to Contribute to this Project?
-------------------------------

Go to http://code.google.com and search for 'MilpLogic'.
Search for MilpLogic.

Git clone sources.
Change sources.
Git push changed sources.



Questions?
----------

sels
dot
peter 
at
gmail
dot 
com
