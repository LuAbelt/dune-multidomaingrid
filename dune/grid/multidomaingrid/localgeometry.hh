#ifndef DUNE_MULTIDOMAINGRID_LOCALGEOMETRY_HH
#define DUNE_MULTIDOMAINGRID_LOCALGEOMETRY_HH

#include <dune/grid/common/geometry.hh>

namespace Dune {

namespace mdgrid {


template<int mydim, int coorddim, typename GridImp>
class LocalGeometryWrapper
{

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

  typedef typename GridImp::HostGrid::Traits::template Codim<dimension-mydim>::LocalGeometry HostLocalGeometry; //TODO: fix this

public:

  typedef typename HostLocalGeometry::GlobalCoordinate GlobalCoordinate;
  typedef typename HostLocalGeometry::LocalCoordinate LocalCoordinate;
  typedef typename HostLocalGeometry::Volume Volume;
  typedef typename HostLocalGeometry::JacobianInverseTransposed JacobianInverseTransposed;
  typedef typename HostLocalGeometry::JacobianTransposed JacobianTransposed;
  typedef typename HostLocalGeometry::JacobianInverse JacobianInverse;
  typedef typename HostLocalGeometry::Jacobian Jacobian;

  GeometryType type() const {
    return _wrappedLocalGeometry.type();
  }

  int corners() const {
    return _wrappedLocalGeometry.corners();
  }

  bool affine() const {
    return _wrappedLocalGeometry.affine();
  }

  GlobalCoordinate corner(int i) const {
    return _wrappedLocalGeometry.corner(i);
  }

  GlobalCoordinate global(const LocalCoordinate& local) const {
    return _wrappedLocalGeometry.global(local);
  }

  LocalCoordinate local(const GlobalCoordinate& global) const {
    return _wrappedLocalGeometry.local(global);
  }

  bool checkInside(const LocalCoordinate& local) const {
    return _wrappedLocalGeometry.checkInside(local);
  }

  Volume integrationElement(const LocalCoordinate& local) const {
    return _wrappedLocalGeometry.integrationElement(local);
  }

  Volume volume() const {
    return _wrappedLocalGeometry.volume();
  }

  GlobalCoordinate center() const {
    return _wrappedLocalGeometry.center();
  }

  JacobianTransposed jacobianTransposed(const LocalCoordinate& local) const {
    return _wrappedLocalGeometry.jacobianTransposed(local);
  }

  JacobianInverseTransposed jacobianInverseTransposed(const LocalCoordinate& local) const {
    return _wrappedLocalGeometry.jacobianInverseTransposed(local);
  }

  Jacobian jacobian(const LocalCoordinate& local) const
  {
    return _wrappedLocalGeometry.jacobian(local);
  }

  JacobianInverse jacobianInverse(const LocalCoordinate& local) const
  {
    return _wrappedLocalGeometry.jacobianInverse(local);
  }

private:

  const HostLocalGeometry _wrappedLocalGeometry;

  LocalGeometryWrapper(const HostLocalGeometry& wrappedLocalGeometry)
    : _wrappedLocalGeometry(wrappedLocalGeometry)
  {}


};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_LOCALGEOMETRY_HH
