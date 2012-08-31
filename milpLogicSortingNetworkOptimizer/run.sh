# To learn about (optimal) sorting networks, see:
# http://en.wikipedia.org/wiki/Sorting_network

# From this webpage:
# For 1 to 8 inputs optimal sorting networks are known. 
# They have respectively 0, 1, 3, 5, 9, 12, 16 and 19 comparators (Knuth, 1997). 
# The optimal depths for up to 10 inputs are known and they are respectively 0, 1, 3, 3, 5, 5, 6, 6, 7, 7.

# Here, our objective function is the number of comparators.

# Usage: Debug/milpLogicSortingNetworkOptimizer
#  <nSortNetworkInputs(N)>
#  <nComparators(C)>
#  <minimizeComparators(1)IoFeasibility(0)>
#  <gap[0.0..1.0]>

executable=./Debug/milpLogicSortingNetworkOptimizer

$executable 2 1 0 0.0
$executable 3 3 0 0.0
$executable 4 5 0 0.0
$executable 5 9 0 0.0
$executable 6 12 0 0.0

# The ones below take really long, hours, days, ...
# but you're welcome to try them on your supercomputer.

#$executable 7 16 0 0.0
#$executable 8 19 0 0.0

