#ifndef DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITYPOINTER_HH
#define DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITYPOINTER_HH

namespace Dune {

namespace mdgrid {

namespace subdomain {

template<int codim, typename GridImp>
class EntityPointerWrapper
{

  static const int dim = GridImp::dimension;

  struct Invalid {};

public:

  typedef EntityPointerWrapper EntityPointerImp;

  static const int codimension = codim;

  typedef typename GridImp::Traits::template Codim<codim>::Entity Entity;
  typedef EntityPointerWrapper<codim,GridImp> Base;

  typedef typename GridImp::MDGridType::Traits::template Codim<codim>::EntityPointer MultiDomainEntityPointer;
  typedef typename GridImp::MDGridType::Traits::template Codim<codim>::LeafIterator MultiDomainLeafIterator;
  typedef typename GridImp::MDGridType::Traits::template Codim<codim>::LevelIterator MultiDomainLevelIterator;
  typedef typename GridImp::MDGridType::Traits::HierarchicIterator MultiDomainHierarchicIterator;

  EntityPointerWrapper(const GridImp& grid, const MultiDomainEntityPointer& multiDomainEntityPointer) :
    _entityWrapper(grid,multiDomainEntityPointer)
  {}

  EntityPointerWrapper(const GridImp& grid, const MultiDomainLeafIterator& multiDomainEntityPointer) :
    _entityWrapper(grid,multiDomainEntityPointer)
  {}

  EntityPointerWrapper(const GridImp& grid,
                       const MultiDomainLevelIterator& multiDomainEntityPointer) :
    _entityWrapper(grid,multiDomainEntityPointer)
  {}

  EntityPointerWrapper(const GridImp& grid,
                       typename SelectType<codim==0,const MultiDomainHierarchicIterator&,Invalid>::Type multiDomainEntityPointer) :
    _entityWrapper(grid,multiDomainEntityPointer)
  {}

  EntityPointerWrapper(const EntityWrapper<codim,dim,GridImp>& entity) :
    _entityWrapper(entity._grid,entity._multiDomainEntityPointer)
  {}

  bool equals(const EntityPointerWrapper<codim,GridImp>& rhs) const {
    return _entityWrapper.multiDomainEntityPointer() == rhs._entityWrapper.multiDomainEntityPointer();
  }

  Entity& dereference() const {
    return _entityWrapper;
  }

  void compactify() {
    _entityWrapper.compactify();
  }

  int level() const {
    return _entityWrapper.level();
  }

protected:

  mutable MakeableEntityWrapper<codim,dim,GridImp> _entityWrapper;

};

} // namespace subdomain

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITYPOINTER_HH
