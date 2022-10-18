#ifndef DUNE_MULTIDOMAINGRID_GEOMETRY_HH
#define DUNE_MULTIDOMAINGRID_GEOMETRY_HH

#include <dune/grid/common/geometry.hh>

namespace Dune {

namespace mdgrid {

template<int mydim, int coorddim, typename GridImp>
class GeometryWrapper
{

  template<int,int,typename>
  friend class EntityWrapperBase;

  template<int,int,typename>
  friend class EntityWrapper;

  template<typename,typename>
  friend class IntersectionWrapper;

  template<typename,typename,typename,typename>
  friend class SubDomainInterface;

public:

  typedef typename GridImp::ctype ctype;
  static const int dimension = GridImp::dimension;
  static const int dimensionworld = GridImp::dimensionworld;
  static const int mydimension = mydim;
  static const int coorddimension = coorddim;

private:

  typedef typename GridImp::HostGrid::Traits::template Codim<dimension-mydim>::Geometry HostGeometry; //TODO: fix this

public:

  typedef typename HostGeometry::GlobalCoordinate GlobalCoordinate;
  typedef typename HostGeometry::LocalCoordinate LocalCoordinate;
  typedef typename HostGeometry::Volume Volume;
  typedef typename HostGeometry::JacobianInverseTransposed JacobianInverseTransposed;
  typedef typename HostGeometry::JacobianTransposed JacobianTransposed;
  typedef typename HostGeometry::JacobianInverse JacobianInverse;
  typedef typename HostGeometry::Jacobian Jacobian;

  GeometryType type() const {
    return _wrappedGeometry.type();
  }

  int corners() const {
    return _wrappedGeometry.corners();
  }

  bool affine() const {
    return _wrappedGeometry.affine();
  }

  GlobalCoordinate corner(int i) const {
    return _wrappedGeometry.corner(i);
  }

  GlobalCoordinate global(const LocalCoordinate& local) const {
    return _wrappedGeometry.global(local);
  }

  LocalCoordinate local(const GlobalCoordinate& global) const {
    return _wrappedGeometry.local(global);
  }

  bool checkInside(const LocalCoordinate& local) const {
    return _wrappedGeometry.checkInside(local);
  }

  Volume integrationElement(const LocalCoordinate& local) const {
    return _wrappedGeometry.integrationElement(local);
  }

  Volume volume() const {
    return _wrappedGeometry.volume();
  }

  GlobalCoordinate center() const {
    return _wrappedGeometry.center();
  }

  JacobianTransposed jacobianTransposed(const LocalCoordinate& local) const {
    return _wrappedGeometry.jacobianTransposed(local);
  }

  JacobianInverseTransposed jacobianInverseTransposed(const LocalCoordinate& local) const {
    return _wrappedGeometry.jacobianInverseTransposed(local);
  }

  Jacobian jacobian(const LocalCoordinate& local) const
  {
    return _wrappedGeometry.jacobian(local);
  }

  JacobianInverse jacobianInverse(const LocalCoordinate& local) const
  {
    return _wrappedGeometry.jacobianInverse(local);
  }

private:

  GeometryWrapper(const HostGeometry& wrappedGeometry)
    : _wrappedGeometry(wrappedGeometry)
  {}

  const HostGeometry _wrappedGeometry;

};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_GEOMETRY_HH
