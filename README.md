MultiDomainGrid
===============

[dune-multidomaingrid][1] is a meta grid built on top of the [DUNE][2]
grid interface. It can be used to carve out subdomains from an underlying
host grid which are then available as fully featured DUNE grids in their
own right.


Version
-------

This is version 2.7-git of MultiDomainGrid. It is compatible with the 2.7-git
release of the DUNE core modules. The versioning scheme of MultiDomainGrid
has recently been synchronized to that of the core modules to make it easier for users
to find a compatible release for their DUNE distribution.

An overview of changes to the library can be found in the file
[CHANGELOG.md][13].


Features
--------

* Support for arbitrary subdomain topologies, including non-contiguous domains.

* Full support for MPI-parallel computations on the subdomains.

* Policy-driven choice of different storage backends tailored to the application
  scenario. The library currently ships with three different implementations:

  * A version optimized for a small number of domains that may overlap in arbitrary
    ways and that maintains O(1) storage and runtime complexity irrespective of the
    amount of subdomain overlap. This backend is best suited to standard multi-physics
    applications that combine a small number of regions with different physics.

  * A version optimized for a very large number of subdomains, with the total number
    known at compile time. This implementation places a compile-time limit on the
    number of subdomains that a grid entity may belong to; several of its algorithms
    are O(log N) in the number of overlapping subdomains N.

  * A variant of the former with a run-time configurable number of total subdomains.
    This version traits added flexibility with a slightly less optimal storage scheme.
    Apart from the switch from a compile-time to a run-time subdomain limit, it is mostly
    identical to the second implementation in terms of algorithmic and storage complexity.

* The subdomain layout can be modified during a simulation, with an API that aids in
  transferring solution data similar to the grid adaptivity interface of a DUNE grid.

* DUNE grid API extensions to query the subdomain membership of a given entity as well
  as conversion methods between `MultiDomainGrid` entities and `SubDomainGrid` entities.

* Iterators for automatic extraction of subdomain - subdomain interfaces, both for a
  given pair of subdomains and for all defined subdomains at once (i.e. in a single host
  grid traversal).

If you have downloaded a release tarball, you can find the autogenerated Doxygen
API documentation in doc/doxygen/html. Otherwise, you can build this documentation
yourself by calling "make doc". Note that you need Doxygen and GraphViz available at
configure time to be able to build the documentation.

If you need help, please ask via [DUNE GitLab][1]. If you find bugs, you can also submit
them to the [bugtracker][3]. Even better, if you have managed to fix a problem, open
a [pull request][4] to get your patch merged into the library.


High-level interface for multi-domain simulations
-------------------------------------------------

While dune-multidomaingrid is a very useful tool in its own right and is used in
a standalone fashion by a number of people, it was originally designed as a building
block for extending the high-level DUNE-based PDE solver toolbox [PDELab][5] with
support for multi-physics and multi-domain simulations. This support is contained in
the [dune-multidomain][6] library, which is an add-on module for PDELab that extends
the latter with concepts for subproblems and couplings between those subproblems and
uses MultiDomainGrid to provide the spatial layout of those subproblems and couplings.


Dependencies
------------

dune-multidomaingrid depends on the following software packages:

* [DUNE core libraries][2] (dune-common, dune-geometry, dune-grid) version 2.7-git,
  and their respective dependencies.

* CMake 3.1.0 and a compiler that is compatible with GCC 5 or newer in C++14 mode.


License
-------

The MultiDomainGrid library, headers and test programs are free open-source software,
dual-licensed under version 3 or later of the GNU Lesser General Public License
and version 2 of the GNU General Public License with a [special run-time exception][8].

See the file [COPYING.md][9] for full copying permissions.


Installation
------------

For a full explanation of the DUNE installation process please read
the [installation notes][11] or the [build system HOWTO][12].


Links
-----

[1]:  https://gitlab.dune-project.org/extensions/dune-multidomaingrid
[2]:  http://dune-project.org
[3]:  https://gitlab.dune-project.org/extensions/dune-multidomaingrid/issues
[4]:  https://gitlab.dune-project.org/extensions/dune-multidomaingrid/merge_requests
[5]:  http://dune-project.org/pdelab/
[6]:  http://github.com/smuething/dune-multidomain
[7]:  https://dune-project.org/modules/dune-pdelab/
[8]:  http://gcc.gnu.org/onlinedocs/libstdc++/faq.html#faq.license
[9]:  COPYING.md
[11]: https://dune-project.org/doc/installation/
[12]: https://dune-project.org/buildsystem/
[13]: CHANGELOG.md
