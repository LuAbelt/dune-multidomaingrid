#ifndef DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LEAFITERATOR_HH
#define DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LEAFITERATOR_HH

namespace Dune {

namespace mdgrid {

namespace subdomain {

template<typename MDGrid>
class SubDomainGrid;

template<int codim, PartitionIteratorType pitype, typename GridImp>
class LeafIteratorWrapper :
    public EntityPointerWrapper<codim,GridImp>
{

  template<typename MDGrid>
  friend class SubDomainGrid;

  template<int, PartitionIteratorType, class,
	   template<int,PartitionIteratorType,class> class>
  friend class LeafIterator;

  template<typename,typename>
  friend class EntityPointer;

  typedef typename GridImp::MultiDomainGrid::Traits::template Codim<codim>::template Partition<pitype>::LeafIterator MultiDomainLeafIterator;
  typedef typename GridImp::Traits::LeafIndexSet IndexSet;

  LeafIteratorWrapper(const IndexSet& indexSet, const MultiDomainLeafIterator multiDomainIterator, const MultiDomainLeafIterator endIterator) :
    EntityPointerWrapper<codim,GridImp>(indexSet._grid,multiDomainIterator),
    _indexSet(indexSet),
    _multiDomainIterator(multiDomainIterator),
    _end(endIterator)
  {
    incrementToNextValidPosition();
  }

  void incrementToNextValidPosition() {
    while(_multiDomainIterator != _end && !_indexSet.containsMultiDomainEntity(*_multiDomainIterator)) {
      ++_multiDomainIterator;
    }
    this->_entityWrapper.reset(_multiDomainIterator);
  }

  void increment() {
    ++_multiDomainIterator;
    incrementToNextValidPosition();
  }


  const LeafIteratorWrapper& operator=(const LeafIteratorWrapper& rhs) {
    assert(_indexSet == rhs._indexSet);
    _multiDomainIterator = rhs._multiDomainIterator;
    _end = rhs._end;
  }

  const IndexSet& _indexSet;
  MultiDomainLeafIterator _multiDomainIterator;
  MultiDomainLeafIterator _end;

};

} // namespace subdomain

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LEAFITERATOR_HH
