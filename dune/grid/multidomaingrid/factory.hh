#ifndef DUNE_MULTIDOMAINGRID_FACTORY_HH
#define DUNE_MULTIDOMAINGRID_FACTORY_HH

namespace Dune {

namespace mdgrid {

// forward declaration
template<typename HostGrid,typename MDGridTraits>
class MultiDomainGrid;

} // namespace mdgrid

// forward declaration
template <class GridType>
class GridFactoryInterface;

template <class GridType>
class GridFactory;

template<class HostGrid, class MDGTraits>
class GridFactory<Dune::mdgrid::MultiDomainGrid<HostGrid, MDGTraits>> : public GridFactoryInterface<Dune::mdgrid::MultiDomainGrid<HostGrid, MDGTraits>> {

  using Grid = Dune::mdgrid::MultiDomainGrid<HostGrid, MDGTraits>;
  using Base = GridFactoryInterface<Grid>;
public:

  GridFactory()
    : _host_grid_factory{}
  {}

  // use default implementation from base class
  using Base::insertBoundarySegment;

  void insertVertex(const FieldVector<typename Grid::ctype, Grid::dimensionworld>& pos) override {
    _host_grid_factory.insertVertex(pos);
  }

  void insertElement(const GeometryType& type, const std::vector<unsigned int>& vertices) override {
    _host_grid_factory.insertElement(type, vertices);
  }

  void insertBoundarySegment(const std::vector<unsigned int>& vertices) override {
    _host_grid_factory.insertBoundarySegment(vertices);
  }

  std::unique_ptr<Grid> createGrid() override  {
    if (not _grid_ptr)
      makeGrid();
    assert(_grid_ptr);
    return std::exchange(_grid_ptr, nullptr);
  }

  void makeGrid(std::size_t max_subdomains = 1) {
    assert(not _grid_ptr);

    std::unique_ptr<MDGTraits> traits;
    if constexpr (std::is_default_constructible_v<MDGTraits>)
      traits = std::make_unique<MDGTraits>();
    else
      traits = std::make_unique<MDGTraits>(max_subdomains);

    if (traits->maxSubDomainIndex() < max_subdomains)
      DUNE_THROW(IOError, "Maximum number of sub-domains is bigger than what grid traits allows");

    _grid_ptr = std::make_unique<Grid>(_host_grid_factory.createGrid(), *traits);
  }

  Grid& grid() {
    assert(_grid_ptr);
    return *_grid_ptr;
  }

  HostGrid& hostGrid() {
    assert(_host_grid_ptr);
    return *_host_grid_ptr;
  }

  const std::shared_ptr<HostGrid>& hostGridPtr() {
    return _host_grid_ptr;
  }


  Dune::GridFactory<HostGrid>& hostGridFactory() {
    return _host_grid_factory;
  }

private:
  Dune::GridFactory<HostGrid> _host_grid_factory;
  std::shared_ptr<HostGrid> _host_grid_ptr;
  std::unique_ptr<Grid> _grid_ptr;
};

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_FACTORY_HH
