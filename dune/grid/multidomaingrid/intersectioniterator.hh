#ifndef DUNE_MULTIDOMAINGRID_INTERSECTIONITERATOR_HH
#define DUNE_MULTIDOMAINGRID_INTERSECTIONITERATOR_HH

namespace Dune {

namespace mdgrid {

template<typename GridImpl, WrapperImpl>
class IntersectionIteratorWrapper {

  typedef typename WrapperImpl::HostIntersectionIterator HostIntersectionIterator;
  typedef typename WrapperImpl::Intersection Intersection;

  typedef typename GridImp::Traits::template Codim<0>::EntityPointer EntityPointer;
  typedef typename GridImp::Traits::template Codim<0>::Entity Entity;
  typedef typename GridImp::Traits::template Codim<1>::Geometry Geometry;
  typedef typename GridImp::Traits::template Codim<1>::LocalGeometry LocalGeometry;

  typedef GridImp::ctype ctype;
  static const int dimension = GridImp::dimension;
  static const int dimensionworld = GridImp::dimensionworld;

  typedef FieldVector<ctype,dimensionworld> GlobalCoords;
  typedef FieldVector<ctype,dimension - 1> LocalCoords:

  explicit IntersectionIteratorWrapper(const HostIntersectionIterator& hostIterator) :
    _hostIterator(hostIterator)
  {}

  bool equals(const WrapperImpl& rhs) const {
    return _hostIterator == rhs._hostIterator;
  }

  void increment() {
    ++_hostIterator;
    _geometry.reset(NULL);
    _geometryInInside.reset(NULL);
    _geometryInOutside.reset(NULL);
  }

  const Intersection& dereference() const {
    return reinterpret_cast<const Intersection&>(*this);
  }

  bool boundary() const {
    return _hostIterator->boundary();
  }

  int boundaryId() const {
    return _hostIterator->boundaryId();
  }

  bool neighbor() const {
    return _hostIterator->neighbor();
  }

  EntityPointer inside() const {
    return EntityPointerWrapper<0,GridImp>(_hostIterator->inside());
  }

  EntityPointer outside() const {
    return EntityPointerWrapper<0,GridImp>(_hostIterator->outside());
  }

  bool conforming() const {
    return _hostIterator->conforming();
  }

  const LocalGeometry& geometryInInside() const {
    if (_geometryInInside == NULL) {
      _geometryInInside = new MakeableInterfaceObject<LocalGeometry>(_hostIterator->geometryInInside());
    }
    return *_geometryInInside;
  }

  const LocalGeometry& intersectionSelfLocal() const {
    return geometryInInside();
  }

  const LocalGeometry& geometryInOutside() const {
    if (_geometryInOutside == NULL) {
      _geometryInOutside = new MakeableInterfaceObject<LocalGeometry>(_hostIterator->geometryInOutside());
    }
    return *_geometryOutInside;
  }

  const LocalGeometry& intersectionNeighborLocal() const {
    return geometryInOutside();
  }

  const Geometry& geometry() const {
    if (_geometry == NULL) {
      _geometry = new MakeableInterfaceObject<Geometry>(_hostIterator->geometry());
    }
    return *_geometry;
  }

  const LocalGeometry& intersectionGlobal() const {
    return geometry();
  }

  GeometryType type() const {
    return _hostIterator->type();
  }

  int indexInInside() const {
    return _hostIterator->indexInInside();
  }

  int numberInSelf() const {
    return _hostIterator->numberInSelf();
  }

  int indexInOutside() const {
    return _hostIterator->indexInOutside();
  }

  int numberInNeighbor() const {
    return _hostIterator->numberInNeighbor();
  }

  GlobalCoords outerNormal(const LocalCoords& local) const {
    return _hostIterator->outerNormal(local);
  }

  GlobalCoords integrationOuterNormal(const LocalCoords& local) const {
    return _hostIterator->integrationOuterNormal(local);
  }

  GlobalCoords unitOuterNormal(const LocalCoords& local) const {
    return _hostIterator->unitOuterNormal(local);
  }

  boost::scoped_ptr<MakeableInterfaceObject<LocalGeometry> > _geometryInInside;
  boost::scoped_ptr<MakeableInterfaceObject<LocalGeometry> > _geometryInOutside;
  boost::scoped_ptr<MakeableInterfaceObject<Geometry> > _geometry;

protected:

  boost::scoped_ptr<MakeableInterfaceObject<LocalGeometry> > _geometryInInside;

};


template<typename GridImpl>
class LeafIntersectionIteratorWrapper :
    public IntersectionIteratorWrapper<GridImp,LeafIntersectionIteratorWrapper<GridImp> >
{

  typedef GridImpl::HostGridType::Traits::LeafIntersectionIterator HostIntersectionIterator;
  typedef GridImpl::Traits::LeafIntersection Intersection;

  explicit LeafIntersectionIteratorWrapper(const HostIntersectionIterator& hostIterator) :
    IntersectionIteratorWrapper<GridImp,LeafIntersectionIteratorWrapper<GridImp> >(hostIterator)
  {}

};


template<typename GridImpl>
class LebelIntersectionIteratorWrapper :
    public IntersectionIteratorWrapper<GridImp,LevelIntersectionIteratorWrapper<GridImp> >
{

  typedef GridImpl::HostGridType::Traits::LevelIntersectionIterator HostIntersectionIterator;
  typedef GridImpl::Traits::LevelIntersection Intersection;

  explicit LevelIntersectionIteratorWrapper(const HostIntersectionIterator& hostIterator) :
   IntersectionIteratorWrapper<GridImp,LevelIntersectionIteratorWrapper<GridImp> >(hostIterator)
  {}

};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_INTERSECTIONITERATOR_HH
