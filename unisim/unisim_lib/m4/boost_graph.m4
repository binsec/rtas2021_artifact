## UNISIM_CHECK_BOOST_GRAPH
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_GRAPH], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])
