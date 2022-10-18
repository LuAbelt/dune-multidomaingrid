#ifndef DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_GEOMETRY_HH
#define DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_GEOMETRY_HH

#include <dune/grid/common/geometry.hh>

namespace Dune {

namespace mdgrid {

namespace subdomain {

template<int mydim, int coorddim, typename GridImp>
class GeometryWrapper
{

  template<int,int,typename>
  friend class EntityWrapperBase;

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
    return _hostGeometry.type();
  }

  int corners() const {
    return _hostGeometry.corners();
  }

  bool affine() const {
    return _hostGeometry.affine();
  }

  GlobalCoordinate corner(int i) const {
    return _hostGeometry.corner(i);
  }

  GlobalCoordinate global(const LocalCoordinate& local) const {
    return _hostGeometry.global(local);
  }

  LocalCoordinate local(const GlobalCoordinate& global) const {
    return _hostGeometry.local(global);
  }

  bool checkInside(const LocalCoordinate& local) const {
    return _hostGeometry.checkInside(local);
  }

  Volume integrationElement(const LocalCoordinate& local) const {
    return _hostGeometry.integrationElement(local);
  }

  Volume volume() const {
    return _hostGeometry.volume();
  }

  GlobalCoordinate center() const {
    return _hostGeometry.center();
  }

  JacobianTransposed jacobianTransposed(const LocalCoordinate& local) const {
    return _hostGeometry.jacobianTransposed(local);
  }

  JacobianInverseTransposed jacobianInverseTransposed(const LocalCoordinate& local) const {
    return _hostGeometry.jacobianInverseTransposed(local);
  }

  Jacobian jacobian(const LocalCoordinate& local) const
  {
    return _hostGeometry.jacobian(local);
  }

  JacobianInverse jacobianInverse(const LocalCoordinate& local) const
  {
    return _hostGeometry.jacobianInverse(local);
  }


private:

  const HostGeometry _hostGeometry;

  GeometryWrapper(const HostGeometry& hostGeometry)
    : _hostGeometry(hostGeometry)
  {}

};

} // namespace subdomain

} // namespace mdgrid


} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_SUBDOMAINGRID_GEOMETRY_HH
