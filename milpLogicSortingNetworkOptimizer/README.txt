

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

