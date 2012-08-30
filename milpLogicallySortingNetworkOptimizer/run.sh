# To learn about (optimal) sorting networks, see:
# http://en.wikipedia.org/wiki/Sorting_network

# From this webpage:
# For 1 to 8 inputs optimal sorting networks are known. 
# They have respectively 0, 1, 3, 5, 9, 12, 16 and 19 comparators (Knuth, 1997). 
# The optimal depths for up to 10 inputs are known and they are respectively 0, 1, 3, 3, 5, 5, 6, 6, 7, 7.

# Here, our objective function is the number of comparators.

executable=./Debug/milpLogicallySortingNetwork

$executable 2 1 1 0.0



