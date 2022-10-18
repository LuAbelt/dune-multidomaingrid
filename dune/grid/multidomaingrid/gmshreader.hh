#ifndef DUNE_MULTIDOMAINGRID_GMSHREADERPARSER_HH
#define DUNE_MULTIDOMAINGRID_GMSHREADERPARSER_HH

namespace Dune {

namespace mdgrid {

// forward declaration
template<typename HostGrid,typename MDGridTraits>
class MultiDomainGrid;

} // namespace mdgrid

// forward declaration
template<typename GridType>
class GmshReaderParser;

template<class HostGrid, class MDGTraits>
class GmshReaderParser<Dune::mdgrid::MultiDomainGrid<HostGrid, MDGTraits>> : public Dune::GmshReaderParser<HostGrid> {
public:

  using Grid = Dune::mdgrid::MultiDomainGrid<HostGrid, MDGTraits>;

  GmshReaderParser(Dune::GridFactory<Grid>& factory, bool v, bool i)
    : Dune::GmshReaderParser<HostGrid>{factory.hostGridFactory(), v, i}
    , _factory{factory}
  {}

  void read (const std::string& f)
  {
    Dune::GmshReaderParser<HostGrid>::read(f);
    const auto& index_map = this->elementIndexMap();
    int max_subdomains = *std::max_element(begin(index_map), end(index_map));
    _factory.makeGrid(max_subdomains);
    auto& grid = _factory.grid();

    grid.startSubDomainMarking();
    unsigned int i = 0;
    for (const auto& cell : elements(grid.leafGridView())) {
      auto subdomain = index_map[i] - 1; // gmsh index starts at 1!
      grid.addToSubDomain(subdomain, cell), i++;
    }

    grid.preUpdateSubDomains();
    grid.updateSubDomains();
    grid.postUpdateSubDomains();
  }

private:
  Dune::GridFactory<Grid>& _factory;
};

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_GMSHREADERPARSER_HH
