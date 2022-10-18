#ifndef DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LOCALGEOMETRY_HH
#define DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LOCALGEOMETRY_HH

#include <dune/grid/common/geometry.hh>

namespace Dune {

namespace mdgrid {

namespace subdomain {

template<int mydim, int coorddim, typename GridImp>
class LocalGeometryWrapper
{

  template<int,int,typename>
  friend class EntityWrapper;

  template<typename,typename,typename>
  friend class IntersectionWrapper;

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
    return _hostLocalGeometry.type();
  }

  int corners() const {
    return _hostLocalGeometry.corners();
  }

  bool affine() const {
    return _hostLocalGeometry.affine();
  }

  GlobalCoordinate corner(int i) const {
    return _hostLocalGeometry.corner(i);
  }

  GlobalCoordinate global(const LocalCoordinate& local) const {
    return _hostLocalGeometry.global(local);
  }

  LocalCoordinate local(const GlobalCoordinate& global) const {
    return _hostLocalGeometry.local(global);
  }

  bool checkInside(const LocalCoordinate& local) const {
    return _hostLocalGeometry.checkInside(local);
  }

  Volume integrationElement(const LocalCoordinate& local) const {
    return _hostLocalGeometry.integrationElement(local);
  }

  Volume volume() const {
    return _hostLocalGeometry.volume();
  }

  GlobalCoordinate center() const {
    return _hostLocalGeometry.center();
  }

  JacobianTransposed jacobianTransposed(const LocalCoordinate& local) const {
    return _hostLocalGeometry.jacobianTransposed(local);
  }

  JacobianInverseTransposed jacobianInverseTransposed(const LocalCoordinate& local) const {
    return _hostLocalGeometry.jacobianInverseTransposed(local);
  }

  Jacobian jacobian(const LocalCoordinate& local) const
  {
    return _hostLocalGeometry.jacobian(local);
  }

  JacobianInverse jacobianInverse(const LocalCoordinate& local) const
  {
    return _hostLocalGeometry.jacobianInverse(local);
  }

private:

  const HostLocalGeometry _hostLocalGeometry;

  LocalGeometryWrapper(const HostLocalGeometry& hostLocalGeometry)
    : _hostLocalGeometry(hostLocalGeometry)
  {}

};

} // namespace subdomain

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_LOCALGEOMETRY_HH
