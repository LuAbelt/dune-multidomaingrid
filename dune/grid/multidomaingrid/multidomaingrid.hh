#ifndef DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH
#define DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH

#include <string>

#include <dune/grid/multidomaingrid/geometry.hh>
#include <dune/grid/multidomaingrid/entity.hh>
#include <dune/grid/multidomaingrid/entitypointer.hh>
#include <dune/grid/multidomaingrid/leafiterator.hh>
#include <dune/grid/multidomaingrid/leveliterator.hh>
#include <dune/grid/multidomaingrid/hierarchiciterator.hh>
#include <dune/grid/multidomaingrid/intersectioniterator.hh>
#include <dune/grid/multidomaingrid/idsets.hh>
#include <dune/grid/multidomaingrid/indexsets.hh>


namespace Dune {

namespace mdgrid {

template<typename HostGrid>
class MultiDomainGrid;

template<typename HostGrid>
struct MultiDomainGridFamily {

  typedef GridTraits<
    HostGrid::dimension,
    HostGrid::dimensionworld,
    MultiDomainGrid<HostGrid>,
    GeometryWrapper,
    EntityWrapper,
    EntityPointerWrapper,
    LevelIteratorWrapper,
    LeafIntersectionIteratorWrapper, // leaf intersection
    LevelIntersectionIteratorWrapper, // level intersection
    LeafIntersectionIteratorWrapper, // leaf intersection iterator
    LevelIntersectionIteratorWrapper, // level intersection iterator
    HierarchicIteratorWrapper,
    LeafIteratorWrapper,
    IndexSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LevelIndexSet>,
    IndexSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LeafIndexSet>,
    IdSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::GlobalIdSet>,
    typename HostGrid::Traits::GlobalIdSet::IdType,
    IdSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LocalIdSet>,
    typename HostGrid::Traits::LocalIdSet::IdType,
    CollectiveCommunication<MultiDomainGrid<HostGrid> >
    > Traits;

};

template<typename HostGrid>
class MultiDomainGrid :
    public GridDefaultImplementation<HostGrid::dimension,
				     HostGrid::dimensionworld,
				     typename HostGrid::ctype,
				     MultiDomainGridFamily<HostGrid> > {


  template<int codim, int dim, typename GridImp>
  friend class EntityWrapper;

  template<int codim, int dim, typename GridImp>
  friend class MakeableEntityWrapper;

  template<int codim, typename GridImp>
  friend class EntityPointerWrapper;

  template<int codim, PartitionIteratorType pitype, typename GridImp>
  friend class LeafIteratorWrapper;

  template<int codim, PartitionIteratorType pitype, typename GridImp>
  friend class LevelIteratorWrapper;

  template<int mydim, int coorddim, typename GridImp>
  friend class GeometryWrapper;

  template<typename GridImp, typename WrappedIndexSet>
  friend class IndexSetWrapper;

  template<typename GridImp, typename WrappedIdSet>
  friend class IdSetWrapper;

  typedef MultiDomainGrid<HostGrid> GridImp;
  typedef HostGrid HostGridType;

  typedef IndexSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LevelIndexSet> LevelIndexSetImp;

  typedef IndexSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LeafIndexSet> LeafIndexSetImp;

  typedef IdSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::GlobalIdSet> GlobalIdSetImp;

  typedef IdSetWrapper<const MultiDomainGrid<HostGrid>, typename HostGrid::Traits::LocalIdSet> LocalIdSetImp;

public:

  typedef MultiDomainGridFamily<HostGrid> GridFamily;
  typedef typename GridFamily::Traits Traits;
  typedef typename HostGrid::ctype ctype;

  explicit MultiDomainGrid(HostGrid& hostGrid) :
    _hostGrid(hostGrid),
    _leafIndexSet(*this),
    _globalIdSet(*this),
    _localIdSet(*this)
  {
    updateIndexSets();
  }

  std::string name() const {
    return "MultiDomainGrid";
  }

  std::size_t maxLevel() const {
    return _hostGrid.maxLevel();
  }

  template<int codim>
  typename Traits::template Codim<codim>::LevelIterator lbegin(int level) const {
    return LevelIteratorWrapper<codim,All_Partition,GridImp>(_hostGrid.lbegin(level));
  }

  template<int codim>
  typename Traits::template Codim<codim>::LevelIterator lend(int level) const {
    return LevelIteratorWrapper<codim,All_Partition,GridImp>(_hostGrid.lend(level));
  }

  template<int codim, PartitionIteratorType PiType>
  typename Traits::template Codim<codim>::template Partition<PiType>::LevelIterator lbegin(int level) const {
    return LevelIteratorWrapper<codim,PiType,GridImp>(_hostGrid.lbegin(level));
  }

  template<int codim, PartitionIteratorType PiType>
  typename Traits::template Codim<codim>::template Partition<PiType>::LevelIterator lend(int level) const {
    return LevelIteratorWrapper<codim,PiType,GridImp>(_hostGrid.lend(level));
  }

  template<int codim>
  typename Traits::template Codim<codim>::LeafIterator leafbegin() const {
    return LeafIteratorWrapper<codim,All_Partition,GridImp>(_hostGrid.leafbegin());
  }

  template<int codim>
  typename Traits::template Codim<codim>::LeafIterator leafEnd() const {
    return LeafIteratorWrapper<codim,All_Partition,GridImp>(_hostGrid.leafend());
  }

  template<int codim, PartitionIteratorType PiType>
  typename Traits::template Codim<codim>::template Partition<PiType>::LeafIterator leafbegin() const {
    return LeafIteratorWrapper<codim,PiType,GridImp>(_hostGrid.leafbegin());
  }

  template<int codim, PartitionIteratorType PiType>
  typename Traits::template Codim<codim>::template Partition<PiType>::LeafIterator leafend() const {
    return LeafIteratorWrapper<codim,PiType,GridImp>(_hostGrid.leafend());
  }

  int size(int level, int codim) const {
    return _hostGrid.size(level,codim);
  }

  int size(int codim) const {
    return _hostGrid.size(codim);
  }

  int size(int level, GeometryType type) const {
    return _hostGrid.size(level,type);
  }

  int size(GeometryType type) const {
    return _hostGrid.size(type);
  }

  const typename Traits::GlobalIdSet& globalIdSet() const {
    return _globalIdSet;
  }

  const typename Traits::LocalIdSet& localIdSet() const {
    return _localIdSet;
  }

  const typename Traits::LevelIndexSet& levelIndexSet(int level) const {
    assert(level <= maxLevel());
    return _levelIndexSets[level];
  }

  const typename Traits::LeafIndexSet& leafIndexSet() const {
    return _leafIndexSet;
  }

  void globalRefine(int refCount) {
    _hostGrid.globalRefine(refCount);
  }

  bool mark(int refCount, const typename Traits::template Codim<0>::Entity& e) {
    return _hostGrid.mark(refCount, getHostEntity(e));
  }

  int getMark(const typename Traits::template Codim<0>::Entity& e) {
    return _hostGrid.getMark(getHostEntity(e));
  }

  bool preAdapt() {
    return _hostGrid.preAdapt();
  }

  bool adapt() {
    bool r = _hostGrid.adapt();
    updateIndexSets();
    return r;
  }

  void postAdapt() {
    _hostGrid.postAdapt();
  }

  int overlapSize(int level, int codim) const {
    return _hostGrid.overlapSize(level,codim);
  }

  int overlapSize(int codim) const {
    return _hostGrid.overlapSize(codim);
  }

  int ghostSize(int level, int codim) const {
    return _hostGrid.ghostSize(level,codim);
  }

  int ghostSize(int codim) const {
    return _hostGrid.ghostSize(codim);
  }

  void updateIndexSets() {
    // make sure we have enough LevelIndexSets
    if (_levelIndexSets.size() <= maxLevel()) {
      _levelIndexSets.resize(maxLevel()+1,LevelIndexSetImp(*this));
    }

    for (int l = 0; l <= maxLevel(); ++l) {
      _levelIndexSets[l].update(_hostGrid.levelIndexSet(l));
    }

    _leafIndexSet.update(_hostGrid.leafIndexSet());

    _globalIdSet.update(_hostGrid.globalIdSet());
    _localIdSet.update(_hostGrid.localIdSet());
  }

private:

  HostGrid& _hostGrid;

  std::vector<typename Traits::LevelIndexSet> _levelIndexSets;
  LeafIndexSetImp _leafIndexSet;

  GlobalIdSetImp _globalIdSet;
  LocalIdSetImp _localIdSet;

  template<typename Entity>
  struct HostEntity {
    typedef typename HostGrid::Traits::template Codim<Entity::codimension>::Entity type;
  };

  template<typename EntityType>
  typename HostEntity<EntityType>::type& hostEntity(EntityType& e) const {
    return getRealImplementation(e).wrappedEntity();
  }

  template<typename EntityType>
  const typename HostEntity<EntityType>::type& hostEntity(const EntityType& e) const {
    return getRealImplementation(e).wrappedEntity();
  }

};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH
