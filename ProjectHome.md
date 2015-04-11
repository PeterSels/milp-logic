What are the Project Goals?

---


It's intended to be used by C++ programmers as a library to make:

(1) the setting up of general MILP (Mixed Integer Linear Programming)
models more **solver independent**
This is done by providing a solver independent general MILP building API in the class 'Solver'.

(2) the setting up of MILP models with lots of **boolean variables and
their logical constraints( (like implication, negation, conjunction,
disjunction)**a lot easier**.
This is done by providing a (again solver independent) boolean logic
variable and constraint addition API in the class 'Solver'.
Internally and automatically the implied linear equations and
inequalities equivalent with the wanted boolean relations are generated.**

(3) the resulting C++ code generating MILP model with this MilpLogic API more readable.

(4) If you want, you can also see the scalability tests in milpLogicSortingNetworkOptimizer
as a solver benchmark. For these milpLogicSortingNetworkOptimizer
models, some solvers solve these models in significantly lower time than others ...