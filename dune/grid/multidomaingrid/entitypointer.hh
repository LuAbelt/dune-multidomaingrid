#ifndef DUNE_MULTIDOMAINGRID_ENTITYPOINTER_HH
#define DUNE_MULTIDOMAINGRID_ENTITYPOINTER_HH

namespace Dune {

namespace mdgrid {

template<int codim, typename GridImp>
class EntityPointerWrapper
{

  static const int dim = GridImp::dimension;

  template<int, int, typename>
  friend class subdomain::EntityWrapper;

  struct Invalid {};

public:

  typedef EntityPointerWrapper EntityPointerImp;

  static const int codimension = codim;

  typedef typename GridImp::Traits::template Codim<codim>::Entity Entity;
  typedef EntityPointerWrapper<codim,GridImp> Base;

  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::EntityPointer HostEntityPointer;
  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::LeafIterator HostLeafIterator;
  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::LevelIterator HostLevelIterator;
  typedef typename GridImp::HostGridType::Traits::HierarchicIterator HostHierarchicIterator;

  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::Entity HostEntity;

  EntityPointerWrapper(const HostEntityPointer& hostEntityPointer) :
    _entityWrapper(hostEntityPointer)
  {}

  EntityPointerWrapper(const HostLeafIterator& hostEntityPointer) :
    _entityWrapper(hostEntityPointer)
  {}

  EntityPointerWrapper(typename SelectType<!is_same<HostLeafIterator,HostLevelIterator>::value,const HostLevelIterator&,Invalid>::Type hostEntityPointer) :
    _entityWrapper(hostEntityPointer)
  {}

  EntityPointerWrapper(typename SelectType<codim==0,const HostHierarchicIterator&,Invalid>::Type hostEntityPointer) :
    _entityWrapper(hostEntityPointer)
  {}

  EntityPointerWrapper(const HostEntity& hostEntity) :
    _entityWrapper(hostEntity)
  {}

  EntityPointerWrapper(const EntityWrapper<codim,dim,GridImp>& entity) :
    _entityWrapper(entity._hostEntityPointer)
  {}

  bool equals(const EntityPointerWrapper<codim,GridImp>& rhs) const {
    return _entityWrapper.hostEntityPointer() == rhs._entityWrapper.hostEntityPointer();
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

  const HostEntityPointer& hostEntityPointer() const {
    return _entityWrapper.hostEntityPointer();
  }

protected:

  mutable MakeableEntityWrapper<codim,dim,GridImp> _entityWrapper;

};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_ENTITYPOINTER_HH
