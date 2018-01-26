#include <config.h>

#include <iostream>

#include <dune/grid/yaspgrid.hh>
#include <dune/grid/multidomaingrid.hh>

#include <dune/grid/test/gridcheck.hh>
#include <dune/grid/test/checkcommunicate.hh>
#include <dune/grid/test/checkgeometryinfather.hh>
#include <dune/grid/test/checkintersectionit.hh>


namespace Dune {

  // Disable boundary segment index check for subdomains
  template<typename HostGrid, typename MDGridTraits>
  struct EnableBoundarySegmentIndexCheck<
    Dune::mdgrid::subdomain::SubDomainGrid<
      Dune::mdgrid::MultiDomainGrid<
        HostGrid,
        MDGridTraits
        >
      >
    >
    : public std::false_type
  {};

}

template <int dim>
void check_grid(std::size_t cells_per_dim) {

  typedef Dune::YaspGrid<dim> HostGrid;
  Dune::FieldVector<typename HostGrid::ctype,dim> lengths(1.0);
  for (unsigned int i = 0; i < dim; i++)
    lengths[i] = i + 1;
  std::array<int,dim> N;
  std::fill(N.begin(), N.end(), cells_per_dim);
  HostGrid wgrid(lengths, N);

  typedef Dune::MultiDomainGrid<HostGrid,Dune::mdgrid::FewSubDomainsTraits<dim,4> > MDGrid;

  MDGrid grid(wgrid);

  grid.globalRefine(2);

  gridcheck(grid);

  auto gv = grid.leafGridView();

  grid.startSubDomainMarking();
  for (auto&& cell : elements(gv)) {
    auto geo = cell.geometry();
    auto c = geo.global(referenceElement(geo).position(0,0));
    double x = c[0];
    double y = dim > 1 ? c[1] : 0.5;
    if (x > 0.2) {
      if (y > 0.3 && y < 0.7) {
        if (x < 0.8)
          grid.addToSubDomain(1,cell);
        if (x > 0.6)
          grid.addToSubDomain(0,cell);
      } else {
        grid.addToSubDomain(0,cell);
      }
    }
  }
  grid.preUpdateSubDomains();
  grid.updateSubDomains();
  grid.postUpdateSubDomains();

#if CHECK_MULTIDOMAINGRID

  // check communication interface
  checkCommunication(grid,-1,Dune::dvverb);
  for(int l=0; l<=grid.maxLevel(); ++l)
    checkCommunication(grid,l,Dune::dvverb);

  // check the method geometryInFather()
  checkGeometryInFather(grid);
  // check the intersection iterator and the geometries it returns
  checkIntersectionIterator(grid);

#endif // CHECK_MULTIDOMAINGRID

#if CHECK_SUBDOMAINGRID

  gridcheck(grid.subDomain(0));
  checkGeometryInFather(grid.subDomain(0));
  checkIntersectionIterator(grid.subDomain(0));

  gridcheck(grid.subDomain(1));
  checkGeometryInFather(grid.subDomain(1));
  checkIntersectionIterator(grid.subDomain(1));

#endif // CHECK_SUBDOMAINGRID

}

int main (int argc , char **argv) {
  try {

    Dune::MPIHelper::instance(argc,argv);

    check_grid<CHECK_DIMENSION>(CHECK_GRIDSIZE);

  } catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }

  return 0;
}
