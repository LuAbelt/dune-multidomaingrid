#ifndef DUNE_MULTIDOMAINGRID_INDEXSETS_HH
#define DUNE_MULTIDOMAINGRID_INDEXSETS_HH

namespace Dune {

namespace mdgrid {

template<typename GridImp, typename WrappedIndexSet>
class IndexSetWrapper :
    public Dune::IndexSet<GridImp,IndexSetWrapper<GridImp,WrappedIndexSet>,
		       typename WrappedIndexSet::IndexType>
{

  typedef typename remove_const<GridImp>::type::HostGridType HostGrid;
  typedef typename remove_const<GridImp>::type::Traits::template Codim<0>::Entity Codim0Entity;

public:

  typedef WrappedIndexSet::IndexType IndexType;
  static const int dimension = remove_const<GridImp>::type::dimension;

  template<int codim>
  IndexType index(const typename remove_const<GridImp>::type::Traits::template Codim<codim>::Entity& e) const {
    return _wrappedIndexSet.index(_grid.hostEntity(e));
  }

  template<typename Entity>
  IndexType index(const Entity& e) const {
    return _wrappedIndexSet.index(_grid.hostEntity(e));
  }

  template<int codim>
  IndexType subIndex(const Codim0Entity& e, int i) const {
    return _wrappedIndexSet.subIndex(_grid.hostEntity(e),i,codim);
  }

  IndexType subIndex(const Codim0Entity& e, int i, unsigned int codim) const {
    return _wrappedIndexSet.subIndex(_grid.hostEntity(e),i,codim);
  }

  const std::vector<GeometryType>& geomTypes(int codim) const {
    return _wrappedIndexSet.geomTypes(codim);
  }

  IndexType size(GeometryType type) const {
    return _wrappedIndexSet.size(type);
  }

  IndexType size(int codim) const {
    return _wrappedIndexSet.size(codim);
  }

  template<typename Entity&>
  bool contains(const Entity& e) const {
    return _wrappedIndexSet.contains(_grid.hostEntity(e));
  }

private:

  const GridImp& _grid;
  const WrappedIndexSet& _wrappedIndexSet;

  IndexSetWrapper(const GridImp& grid, const WrappedIndexSet& wrappedIndexSet) :
    _grid(grid),
    _wrappedIndexSet(wrappedIndexSet)
  {}

};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_INDEXSETS_HH
