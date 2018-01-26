
# grid checks
#
# build separate checks for each dimension, MultiDomainGrid / SubDomainGrid
# and for configurations with and without checking of deprecated APIs
#
# All of these tests are expected to run without generating any warnings, so
# they use -Werror to fail in that case.
#
foreach(dimsize IN ITEMS "1;64" "2;16" "3;4")
  list(GET dimsize 0 dim)
  list(GET dimsize 1 size)

  foreach(gridtype IN ITEMS multidomaingrid subdomaingrid)

    dune_add_test(
      NAME "test-wrapper-${dim}d-${gridtype}"
      SOURCES test-wrapper.cc
      COMPILE_DEFINITIONS
        DISABLE_DEPRECATED_METHOD_CHECK=1
        CHECK_DIMENSION=${dim}
        CHECK_GRIDSIZE=${size}
        "CHECK_$<UPPER_CASE:${gridtype}>"
      COMPILE_FLAGS
        -UNDEBUG # always compile with assertions enabled
      )

    dune_add_test(
      NAME "test-wrapper-${dim}d-${gridtype}-deprecated-api"
      SOURCES test-wrapper.cc
      COMPILE_DEFINITIONS
        CHECK_DIMENSION=${dim}
        CHECK_GRIDSIZE=${size}
        "CHECK_$<UPPER_CASE:${gridtype}>"
      COMPILE_FLAGS
        -UNDEBUG # always compile with assertions enabled
      )

  endforeach()

endforeach()

dune_add_test(
  SOURCES iterateallinterfaces.cc
  CMD_ARGS 3
  )

dune_add_test(SOURCES multidomain-leveliterator-bug)
dune_add_test(SOURCES testadaptation)
dune_add_test(SOURCES testintersectionconversion)
dune_add_test(SOURCES testintersectiongeometrytypes)
dune_add_test(SOURCES testlargedomainnumbers)
dune_add_test(
  SOURCES testparallel
  MPI_RANKS 2
  TIMEOUT 10
  CMD_ARGS 5 1
  )

dune_add_test(SOURCES testpartitioning)
