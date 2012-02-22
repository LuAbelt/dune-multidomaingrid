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
  friend class EntityWrapper;

  template<typename,typename,typename,typename>
  friend class IntersectionWrapper;

public:

  typedef typename GridImp::ctype ctype;
  static const int dimension = GridImp::dimension;
  static const int dimensionworld = GridImp::dimensionworld;
  static const int mydimension = mydim;
  static const int coorddimension = coorddim;

private:

  typedef FieldVector<ctype,coorddimension> GlobalCoords;
  typedef FieldVector<ctype,mydimension> LocalCoords;
  typedef typename GridImp::HostGridType::Traits::template Codim<dimension-mydim>::Geometry HostGeometry; //TODO: fix this

public:

  GeometryType type() const {
    return _hostGeometry.type();
  }

  int corners() const {
    return _hostGeometry.corners();
  }

  bool affine() const {
    return _hostGeometry.affine();
  }

  GlobalCoords corner(int i) const {
    return _hostGeometry.corner(i);
  }

  GlobalCoords global(const LocalCoords& local) const {
    return _hostGeometry.global(local);
  }

  LocalCoords local(const GlobalCoords& global) const {
    return _hostGeometry.local(global);
  }

  bool checkInside(const LocalCoords& local) const {
    return _hostGeometry.checkInside(local);
  }

  ctype integrationElement(const LocalCoords& local) const {
    return _hostGeometry.integrationElement(local);
  }

  ctype volume() const {
    return _hostGeometry.volume();
  }

  GlobalCoords center() const {
    return _hostGeometry.center();
  }

  const FieldMatrix<ctype,mydimension,coorddimension>&
  jacobianTransposed(const LocalCoords& local) const {
    return _hostGeometry.jacobianTransposed(local);
  }

  const FieldMatrix<ctype,coorddimension,mydimension>&
  jacobianInverseTransposed(const LocalCoords& local) const {
    return _hostGeometry.jacobianInverseTransposed(local);
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
