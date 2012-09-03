We give some results here of runs of milpLogicSortingNetworkOptimizer
with different solvers.


(1) Results Meaning
------------------

A result like:
=*=
*+*
+=+
means that there are 3 inputs, 1 at the left of eachr row.
Each column represents an operation on the inputs.
The first column:
=
*
+
means that 
(1) the top input is fed through (=) to the right,
(2) the middle  is the AND (*) of the middle input and the lower input
(since the corresponding + is at that row) and 
(3) the bottom is the OR (+) of the bottom input and the middle (since
the corresponding * is at that row).

The next column, takes the resulting outputs from the first column
operations and applies its own operators on it.
*
+
=
Note that, per column, there is exactly one * and one +, and
the * always occurs above the +. The * and + together are called a
comparator,
because they only compare two inputs and if they are not ordered, they
are swapped. If they were already oredered, they are left alone (not
swapped).


We look for a network with the minimal number of comparators, so
minimal number of columns, that will sort all the outputs in order, with the
lowest one at the top and the highest one at the bottom. Whatever the
inputs are.

There is a Lemma that says that if a sorting network with N inputs (and
N outputs) does sort all the 2^N binary string in 2^|N, then it will
sort also all other sets of 2^N numbers.
Comparators may have to be adapted, but the structure of the network
in terms of comparators (So the +,*,= pattern) remains the same.




(2) Some computational results with different solvers 
------------------------------------------------


-----------------------------------------------------------------------------------------------------------------------
 RESULTS WITH CPLEX 12.4.0.0
-----------------------------------------------------------------------------------------------------------------------

$executable 2 1 0 0.0

Solving with Cplex v12.4.0.0 took:  0.009325s wall, 0.000000s user + 0.000000s system = 0.000000s CPU (n/a%)

goal function value = 1
Optimal comparator sorting network =
*
+

Checking result with all 4 in- and output vectors
Check is OK.

-------------------

$executable 3 3 0 0.0

Solving with Cplex v12.4.0.0 took:  0.030775s wall, 0.030000s user + 0.010000s system = 0.040000s CPU (130.0%)

goal function value = 3
Optimal comparator sorting network =
=*=
*+*
+=+

Checking result with all 8 in- and output vectors
Check is OK.

-------------------

$executable 4 5 0 0.0

Solving with Cplex v12.4.0.0 took:  1.121650s wall, 1.870000s user + 0.050000s system = 1.920000s CPU (171.2%)

goal function value = 5
Optimal comparator sorting network =
=**==
*=+=*
+==*+
=+=+=

Checking result with all 16 in- and output vectors
Check is OK.

-------------------

$executable 5 9 0 0.0

Solving with Cplex v12.4.0.0 took:  111.889726s wall, 452.220000s user + 5.400000s system = 457.620000s CPU (409.0%)

goal function value = 9
Optimal comparator sorting network =
*===*=*==
===*=*+=*
=*===+=*+
++*=+==+=
==++=====

Checking result with all 32 in- and output vectors
Check is OK.


-------------------

$executable 6 12 0 0.0

No results yet after 112521.79s.



-----------------------------------------------------------------------------------------------------------------------
 RESULTS WITH GUROBI v5.0.1
-----------------------------------------------------------------------------------------------------------------------

$executable 2 1 0 0.0

Solving with Gurobi v5.0.1 took:  0.001136s wall, 0.000000s user + 0.000000s system = 0.000000s CPU (n/a%)

goal function value = 1
Optimal comparator sorting network =
*
+

Checking result with all 4 in- and output vectors
Check is OK.

-------------------

$executable 3 3 0 0.0

Optimal comparator sorting network =
*=*
+*+
=+=

Checking result with all 8 in- and output vectors
Check is OK.

-------------------

$executable 4 5 0 0.0

Solving with Gurobi v5.0.1 took:  52.858602s wall, 55.160000s user + 0.860000s system = 56.020000s CPU (106.0%)

goal function value = 5
Optimal comparator sorting network =
=**==
=+=**
*=+=+
+==+=

Checking result with all 16 in- and output vectors
Check is OK.

-------------------

$executable 5 9 0 0.0

Solving with Gurobi v5.0.1 took:  4862.047317s wall, 16802.420000s user + 747.270000s system = 17549.690000s CPU (361.0%)

goal function value = 9
Optimal comparator sorting network =
==***====
*===+=*=*
=*=+===*+
+=+==*++=
=+===+===

Checking result with all 32 in- and output vectors
Check is OK.

-------------------

$executable 6 12 0 0.0

...

Stopped at 94870s









-----------------------------------------------------------------------------------------------------------------------
 RESULTS WITH XPRESS Fico v7.2.1
-----------------------------------------------------------------------------------------------------------------------






----------------------

$executable 3 3 0 0.0


Solving with Xpress v4.4.1 took:  0.030668s wall, 0.000000s user + 0.015600s system = 0.015600s CPU (50.9%)

goal function value = 3
Optimal comparator sorting network =
=*=
*+*
+=+

Checking result with all 8 in- and output vectors
Check is OK.


----------------------

$executable 4 5 0 0.0


The model is solved optimally
Solving with Xpress v4.4.1 took:  4.572897s wall, 6.552042s user + 0.046800s system = 6.598842s CPU (144.3%)

goal function value = 5
Optimal comparator sorting network =
=**==
=+=**
*=+=+
+==+=

Checking result with all 16 in- and output vectors
Check is OK.

----------------------


$executable 5 9 0 0.0

The model is solved optimally
Solving with Xpress v4.4.1 took:  127.281106s wall, 127.624418s user +0.218401s system = 127.842820s CPU (100.4%)

goal function value = 9
Optimal comparator sorting network =
*===*=**=
+*==+*=+=
==*==+==*
=+=*==+=+
==++=====

Checking result with all 32 in- and output vectors
Check is OK.

----------------------

