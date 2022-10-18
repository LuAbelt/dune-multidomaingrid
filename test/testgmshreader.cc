
#include "config.h"
#include <dune/grid/uggrid.hh>
#include <dune/grid/io/file/gmshreader.hh>
#include <dune/grid/multidomaingrid.hh>
#include <iostream>

int main(int argc, char** argv)
{

  try {

    Dune::MPIHelper::instance(argc,argv);

    if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <gmhs-file> <expected-domains>" << std::endl;
      DUNE_THROW(Dune::IOError, "");
    }

    std::string fileName = argv[1];
    std::size_t expectedDomains = atoi(argv[2]);

    using HostGrid = Dune::UGGrid<2>;
    using MDGrid = Dune::MultiDomainGrid<HostGrid,Dune::mdgrid::FewSubDomainsTraits<2,8> >;

    auto mdgrid = Dune::GmshReader<MDGrid>::read(fileName);

    if (mdgrid->maxAssignedSubDomainIndex() + 1 != expectedDomains)
      DUNE_THROW(Dune::InvalidStateException, "Grid does not contain expected number of sub-domains");

  } catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }

}
