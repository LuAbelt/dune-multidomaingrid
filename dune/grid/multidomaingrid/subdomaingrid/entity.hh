#ifndef DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITY_HH
#define DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITY_HH

namespace Dune {

namespace mdgrid {

namespace subdomain {

template<int codim, int dim, typename GridImp>
class EntityWrapper;

template<int codim, typename GridImp>
class EntityPointerWrapper;

template<typename GridImp>
class LeafIntersectionIteratorWrapper;

template<typename GridImp>
class LevelIntersectionIteratorWrapper;

template<typename GridImp>
class HierarchicIteratorWrapper;

template<typename MDGrid>
class SubDomainGrid;

template<int codim, int dim, typename GridImp>
class MakeableEntityWrapper :
    public GridImp::template Codim<codim>::Entity
{

  template<int, typename>
  friend class EntityPointerWrapper;

  template<int, PartitionIteratorType, typename>
  friend class LeafIteratorWrapper;

  template<int, PartitionIteratorType, typename>
  friend class LevelIteratorWrapper;

  template<typename>
  friend class SubDomainGrid;

  template<typename>
  friend class HierarchicIteratorWrapper;

  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::EntityPointer HostEntityPointer;

  MakeableEntityWrapper(const GridImp& grid, const HostEntityPointer& hostEntityPointer) :
    GridImp::template Codim<codim>::Entity(EntityWrapper<codim,dim,const GridImp>(grid,hostEntityPointer))
  {}

  void reset(const HostEntityPointer& hostEntityPointer) {
    this->getRealImp().reset(hostEntityPointer);
  }

  void compactify() {
    this->getRealImp().compactify();
  }

  const HostEntityPointer& hostEntityPointer() const {
    return this->getRealImp().hostEntityPointer();
  }

};


template<int codim, int dim, typename GridImp>
class EntityWrapper :
    public EntityDefaultImplementation<codim,dim,GridImp,EntityWrapper>
{

  template<int, int, typename>
  friend class MakeableEntityWrapper;

  template<int, typename>
  friend class EntityPointerWrapper;

  template<typename>
  friend class SubDomainGrid;

  template<int, int, typename, template<int,int,typename> class>
  friend class Entity;

  typedef typename GridImp::HostGridType::Traits::template Codim<codim>::EntityPointer HostEntityPointer;

public:

  typedef typename GridImp::template Codim<codim>::Geometry Geometry;

  EntityWrapper(const GridImp& grid, const HostEntityPointer& e) :
    _grid(grid),
    _hostEntityPointer(e)
  {}

  int level() const {
    return _hostEntityPointer->level();
  }

  PartitionType partitionType() const {
    return _hostEntityPointer->partitionType();
  }

  template<int cc>
  int count() const {
    return _hostEntityPointer->template count<cc>();
  }

  const Geometry& geometry() const {
    if (!_geometry.isSet()) {
      _geometry.reset(_hostEntityPointer->geometry());
    }
    return _geometry;
  }

  private:

  const GridImp& _grid;
  HostEntityPointer _hostEntityPointer;
  MakeableGeometryWrapper<Geometry::mydimension,Geometry::coorddimension,GridImp> _geometry;

  const EntityWrapper& operator=(const EntityWrapper& rhs) {
    assert(_grid == rhs._grid);
    reset(rhs._hostEntityPointer);
    return *this;
  }

  void reset(const HostEntityPointer& hostEntityPointer) {
    if (_hostEntityPointer != hostEntityPointer) {
      _geometry.clear();
      _hostEntityPointer = hostEntityPointer;
    }
  }

  void compactify() {
    _geometry.clear();
    _hostEntityPointer.compactify();
  }

  const HostEntityPointer& hostEntityPointer() const {
    return _hostEntityPointer;
  }

};


template<int dim, typename GridImp>
class EntityWrapper<0,dim,GridImp> :
    public EntityDefaultImplementation<0,dim,GridImp,EntityWrapper>
{

  template<int, int, typename>
  friend class MakeableEntityWrapper;

  template<int, typename>
  friend class EntityPointerWrapper;

  template<typename>
  friend class SubDomainGrid;

  template<int, int, typename, template<int,int,typename> class>
  friend class Entity;

  typedef typename GridImp::HostGridType::Traits::template Codim<0>::EntityPointer HostEntityPointer;

public:

  typedef typename GridImp::template Codim<0>::Geometry Geometry;
  typedef typename GridImp::template Codim<0>::LocalGeometry LocalGeometry;
  typedef typename GridImp::Traits::LeafIntersectionIterator LeafIntersectionIterator;
  typedef typename GridImp::Traits::LevelIntersectionIterator LevelIntersectionIterator;
  typedef typename GridImp::Traits::HierarchicIterator HierarchicIterator;
  typedef typename GridImp::Traits::template Codim<0>::EntityPointer EntityPointer;


  EntityWrapper(const GridImp& grid, const HostEntityPointer& e) :
    _grid(grid),
    _hostEntityPointer(e)
  {}

  int level() const {
    return _hostEntityPointer->level();
  }

  PartitionType partitionType() const {
    return _hostEntityPointer->partitionType();
  }

  const Geometry& geometry() const {
    if (!_geometry.isSet()) {
      _geometry.reset(_hostEntityPointer->geometry());
    }
    return _geometry;
  }

  template<int cc>
  int count() const {
    return _hostEntityPointer->template count<cc>();
  }

  template<int cc>
  typename GridImp::template Codim<cc>::EntityPointer subEntity(int i) const {
    return EntityPointerWrapper<cc,GridImp>(_grid,_hostEntityPointer->subEntity<cc>(i));
  }

  LeafIntersectionIterator ileafbegin() const {
    return LeafIntersectionIteratorWrapper<GridImp>(_grid,_hostEntityPointer->ileafbegin());
  }

  LeafIntersectionIterator ileafend() const {
    return LeafIntersectionIteratorWrapper<GridImp>(_grid,_hostEntityPointer->ileafend());
  }

  LevelIntersectionIterator ilevelbegin() const {
    return LevelIntersectionIteratorWrapper<GridImp>(_grid,level(),_hostEntityPointer->ilevelbegin());
  }

  LevelIntersectionIterator ilevelend() const {
    return LevelIntersectionIteratorWrapper<GridImp>(_grid,level(),_hostEntityPointer->ilevelend());
  }

  EntityPointer father() const {
    return EntityPointerWrapper<0,GridImp>(_grid,_hostEntityPointer->father());
  }

  bool isLeaf() const {
    return _hostEntityPointer->isLeaf();
  }

  bool isRegular() const {
    return _hostEntityPointer->isRegular();
  }

  const LocalGeometry& geometryInFather() const {
    if (!_fatherGeometry.isSet()) {
      _fatherGeometry.reset(_hostEntityPointer->geometryInFather());
    }
    return _fatherGeometry;
  }

  HierarchicIterator hbegin(int maxLevel) const {
    return HierarchicIteratorWrapper<GridImp>(_grid,
                                              _hostEntityPointer->hbegin(maxLevel),
                                              _hostEntityPointer->hend(maxLevel));
  }

  HierarchicIterator hend(int maxLevel) const {
    return HierarchicIteratorWrapper<GridImp>(_grid,
                                              _hostEntityPointer->hend(maxLevel),
                                              _hostEntityPointer->hend(maxLevel));
  }

  bool isNew() const {
    return _hostEntityPointer->isNew();
  }

  bool mightVanish() const {
    return _hostEntityPointer->mightVanish();
  }

private:

  const GridImp& _grid;
  HostEntityPointer _hostEntityPointer;
  MakeableGeometryWrapper<Geometry::mydimension,Geometry::coorddimension,GridImp> _geometry;
  MakeableGeometryWrapper<LocalGeometry::mydimension,LocalGeometry::coorddimension,GridImp> _fatherGeometry;

  const EntityWrapper& operator=(const EntityWrapper& rhs) {
    assert(_grid == rhs._grid);
    reset(rhs._hostEntityPointer);
    return *this;
  }

  void reset(const HostEntityPointer& hostEntityPointer) {
    if (_hostEntityPointer != hostEntityPointer) {
      _geometry.clear();
      _fatherGeometry.clear();
      _hostEntityPointer = hostEntityPointer;
    }
  }

  void compactify() {
    _geometry.clear();
    _fatherGeometry.clear();
    _hostEntityPointer.compactify();
  }

  const HostEntityPointer& hostEntityPointer() const {
    return _hostEntityPointer;
  }

};

} // namespace subdomain

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_ENTITY_HH