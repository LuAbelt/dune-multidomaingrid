#ifndef DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH
#define DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH

#include <string>
#include <memory>

#include <dune/common/shared_ptr.hh>

#include <dune/grid/common/grid.hh>

#include <dune/grid/multidomaingrid/hostgridaccessor.hh>
#include <dune/grid/multidomaingrid/subdomainset.hh>

#include <dune/grid/multidomaingrid/subdomaingrid/subdomaingrid.hh>

#include <dune/grid/multidomaingrid/geometry.hh>
#include <dune/grid/multidomaingrid/localgeometry.hh>
#include <dune/grid/multidomaingrid/entity.hh>
#include <dune/grid/multidomaingrid/iterator.hh>
#include <dune/grid/multidomaingrid/hierarchiciterator.hh>
#include <dune/grid/multidomaingrid/intersection.hh>
#include <dune/grid/multidomaingrid/intersectioniterator.hh>
#include <dune/grid/multidomaingrid/idsets.hh>
#include <dune/grid/multidomaingrid/indexsets.hh>
#include <dune/grid/multidomaingrid/gridview.hh>
#include <dune/grid/multidomaingrid/mdgridtraits.hh>

#include <dune/grid/multidomaingrid/subdomaintosubdomaininterfaceiterator.hh>
#include <dune/grid/multidomaingrid/allsubdomaininterfacesiterator.hh>

namespace Dune {

namespace mdgrid {

template<typename HostGrid, typename MDGridTraits>
class MultiDomainGrid;

template<typename HostGrid, typename MDGridTraits>
struct MultiDomainGridFamily {

private:

  static const int dim  = HostGrid::dimension;
  static const int dimw = HostGrid::dimensionworld;

public:

  struct Traits
  {
    /** \brief The type that implementing the grid. */
    using Grid = MultiDomainGrid<HostGrid,MDGridTraits>;


    using LeafIntersection = Dune::Intersection<
      const Grid,
      IntersectionWrapper<
        const Grid,
        typename HostGrid::LeafGridView::Intersection
        >
      >;

    using LevelIntersection = Dune::Intersection<
      const Grid,
      IntersectionWrapper<
        const Grid,
        typename HostGrid::LevelGridView::Intersection
        >
      >;

    using LeafIntersectionIterator = Dune::IntersectionIterator<
      const Grid,
      IntersectionIteratorWrapper<
        const Grid,
        typename HostGrid::LeafGridView::IntersectionIterator
        >,
      IntersectionWrapper<
        const Grid,
        typename HostGrid::LeafGridView::Intersection
        >
      >;

    using LevelIntersectionIterator = Dune::IntersectionIterator<
      const Grid,
      IntersectionIteratorWrapper<
        const Grid,
        typename HostGrid::LevelGridView::IntersectionIterator
        >,
      IntersectionWrapper<
        const Grid,
        typename HostGrid::LevelGridView::Intersection
        >
      >;


    using HierarchicIterator = Dune::EntityIterator<
      0,
      const Grid,
      HierarchicIteratorWrapper<
        const Grid
        >
      >;


    template <int cd>
    struct Codim
    {

      using Geometry      = Dune::Geometry<dim-cd, dimw, const Grid, GeometryWrapper>;
      using LocalGeometry = Dune::Geometry<dim-cd, dimw, const Grid, LocalGeometryWrapper>;

      using Entity        = Dune::Entity<cd, dim, const Grid, EntityWrapper>;

      using EntitySeed    = EntitySeedWrapper<typename HostGrid::template Codim<cd>::EntitySeed>;

      template <PartitionIteratorType pitype>
      struct Partition
      {

        using LevelIterator = Dune::EntityIterator<
          cd,
          const Grid,
          IteratorWrapper<
            typename HostGrid::LevelGridView,
            cd,
            pitype,
            const Grid
            >
          >;

        using LeafIterator = Dune::EntityIterator<
          cd,
          const Grid,
          IteratorWrapper<
            typename HostGrid::LeafGridView,
            cd,
            pitype,
            const Grid
            >
          >;
      };

      using LeafIterator  = typename Partition< All_Partition >::LeafIterator;
      using LevelIterator = typename Partition< All_Partition >::LevelIterator;

    private:
      friend class Dune::Entity<cd, dim, const Grid, EntityWrapper>;
    };


    using LevelGridView = Dune::GridView<LevelGridViewTraits<const Grid> >;
    using LeafGridView = Dune::GridView<LeafGridViewTraits<const Grid> >;

    using LevelIndexSet = IndexSetWrapper<const Grid, typename HostGrid::LevelGridView>;
    using LeafIndexSet  = IndexSetWrapper<const Grid, typename HostGrid::LeafGridView>;

    using GlobalIdSet = IdSet<
      const Grid,
      IdSetWrapper<
        const Grid,
        typename HostGrid::Traits::GlobalIdSet
        >,
      typename HostGrid::Traits::GlobalIdSet::IdType
      >;

    using LocalIdSet = IdSet<
      const Grid,
      IdSetWrapper<
        const Grid,
        typename HostGrid::Traits::LocalIdSet
        >,
      typename HostGrid::Traits::LocalIdSet::IdType
      >;

    using CollectiveCommunication
    [[deprecated("Use CommunicationType. Will be removed after release 2.9")]]
    = typename HostGrid::CollectiveCommunication;

    using Communication = typename HostGrid::Communication;

    using LeafSubDomainInterfaceIterator  = Dune::mdgrid::LeafSubDomainInterfaceIterator<const Grid>;
    using LevelSubDomainInterfaceIterator = Dune::mdgrid::LevelSubDomainInterfaceIterator<const Grid>;

    using LeafAllSubDomainInterfacesIterator  = Dune::mdgrid::LeafAllSubDomainInterfacesIterator<const Grid>;
    using LevelAllSubDomainInterfacesIterator = Dune::mdgrid::LevelAllSubDomainInterfacesIterator<const Grid>;

  };

};

namespace Impl {

  template<typename Grid, typename SI, bool max_subdomain_index_is_static>
  struct MaxSubDomainIndexProvider
  {

    typedef SI SubDomainIndex;

    const SubDomainIndex maxSubDomainIndex() const
    {
      return static_cast<const Grid*>(this)->traits().maxSubDomainIndex();
    }

  };

  template<typename Grid, typename SI>
  struct MaxSubDomainIndexProvider<Grid,SI,true>
  {

    typedef SI SubDomainIndex;

    static constexpr SubDomainIndex maxSubDomainIndex()
    {
      return Grid::MDGridTraits::maxSubDomainIndex();
    }

  };

}


//! A meta grid for dividing an existing DUNE grid into subdomains that can be accessed as a grid in their own right.
/**
 * \tparam HostGrid             The type of the underlying grid implementation.
 * \tparam MDGridTraitsType     A traits type for customizing how the MultiDomainGrid manages the partitioning information.
 */

template<
  typename HostGrid_,
  typename MDGridTraitsType
  >
class MultiDomainGrid
  : public GridDefaultImplementation<HostGrid_::dimension,
                                     HostGrid_::dimensionworld,
                                     typename HostGrid_::ctype,
                                     MultiDomainGridFamily<
                                       HostGrid_,
                                       MDGridTraitsType
                                       >
                                     >,
    public Impl::MaxSubDomainIndexProvider<MultiDomainGrid<
                                       HostGrid_,
                                       MDGridTraitsType
                                       >,
                                     typename MDGridTraitsType::SubDomainIndex,
                                     MDGridTraitsType::maxSubDomainIndexIsStatic()
                                     >
{

public:

  using HostGrid = HostGrid_;

private:


  template<int codim, int dim, typename GridImp>
  friend class EntityWrapper;

  template<typename,int,PartitionIteratorType,typename>
  friend class IteratorWrapper;

  template<typename GridImp>
  friend class HierarchicIteratorWrapper;

  template<int mydim, int coorddim, typename GridImp>
  friend class GeometryWrapper;

  template<int mydim, int coorddim, typename GridImp>
  friend class LocalGeometryWrapper;

  template<typename GridImp, typename WrappedIndexSet>
  friend class IndexSetWrapper;

  template<typename GridImp, typename WrappedIdSet>
  friend class IdSetWrapper;

  template<typename GridImp>
  friend struct detail::HostGridAccessor;

  template<typename,typename>
  friend class IntersectionIteratorWrapper;

  template<typename,typename>
  friend class IntersectionWrapper;

  template<typename>
  friend class subdomain::SubDomainGrid;

  template<typename>
  friend struct subdomain::SubDomainGridFamily;

  template<int,int,typename>
  friend class subdomain::EntityWrapperBase;

  template<int,int,typename>
  friend class subdomain::EntityWrapper;

  template <typename>
  friend class LeafSubDomainInterfaceIterator;

  template <typename>
  friend class LevelSubDomainInterfaceIterator;

  template <typename>
  friend class LeafAllSubDomainInterfacesIterator;

  template <typename>
  friend class LevelAllSubDomainInterfacesIterator;

  template<typename,typename,typename,typename>
  friend class SubDomainInterface;

  template<typename,typename,typename>
  friend class subdomain::IntersectionIteratorWrapper;

  template<typename,typename,typename>
  friend class subdomain::IntersectionWrapper;

  template<typename>
  friend class LeafGridView;

  template<typename>
  friend class LevelGridView;

  typedef GridDefaultImplementation<HostGrid::dimension,
                                    HostGrid::dimensionworld,
                                    typename HostGrid::ctype,
                                    MultiDomainGridFamily<HostGrid,MDGridTraitsType>
                                    > BaseT;

  typedef MultiDomainGrid<HostGrid,MDGridTraitsType> GridImp;

  typedef IndexSetWrapper<const GridImp, typename HostGrid::LevelGridView> LevelIndexSetImp;

  typedef IndexSetWrapper<const GridImp, typename HostGrid::LeafGridView> LeafIndexSetImp;

  typedef IdSetWrapper<const GridImp, typename HostGrid::Traits::GlobalIdSet> GlobalIdSetImp;

  typedef IdSetWrapper<const GridImp, typename HostGrid::Traits::LocalIdSet> LocalIdSetImp;

  enum State { stateFixed, stateMarking, statePreUpdate, statePostUpdate, statePreAdapt, statePostAdapt };

  typedef GridImp ThisType;

  using Base = GridDefaultImplementation<
    HostGrid::dimension,
    HostGrid::dimensionworld,
    typename HostGrid::ctype,
    MultiDomainGridFamily<
      HostGrid,
      MDGridTraitsType
      >
    >;

public:

  using Base::dimension;
  using Base::dimensionworld;

  typedef MultiDomainGridFamily<HostGrid,MDGridTraitsType> GridFamily;
  typedef typename GridFamily::Traits Traits;
  typedef MDGridTraitsType MDGridTraits;
  typedef typename HostGrid::ctype ctype;

private:

  typedef std::map<typename Traits::LocalIdSet::IdType,typename MDGridTraits::template Codim<0>::SubDomainSet> AdaptationStateMap;

  typedef std::map<typename Traits::GlobalIdSet::IdType,typename MDGridTraits::template Codim<0>::SubDomainSet> LoadBalanceStateMap;

  // typedefs for extracting the host entity types from our own entities

  template<typename Entity>
  struct HostEntity {
    typedef typename HostGrid::Traits::template Codim<Entity::codimension>::Entity type;
  };

  // typedefs for extracting the multidomain entity types from subdomain entities

  template<typename Entity>
  struct MultiDomainEntity {
    typedef typename Traits::template Codim<Entity::codimension>::Entity type;
  };

public:

  //! The (integer) type used to identify subdomains.
  typedef typename MDGridTraits::SubDomainIndex SubDomainIndex;

  //! The largest number of subdomains any given grid cell may belong to.
  static const std::size_t maxNumberOfSubDomains = MDGridTraits::maxSubDomainsPerCell;

#ifdef DOXYGEN
  //! The largest allowed index for a subdomain.
  /**
   * \note As subdomain indices always start at 0, this also determines the maximum
   * number of possible subdomains.
   */
  const SubDomainIndex maxSubDomainIndex() const
  {
    return _traits.maxSubDomainIndex();
  }
#endif

  static constexpr bool maxSubDomainIndexIsStatic()
  {
    return MDGridTraits::maxSubDomainIndexIsStatic();
  }

  //! The type used for representing the grid of a subdomain, always a specialization of Dune::mdgrid::subdomain::SubDomainGrid.
  typedef subdomain::SubDomainGrid<ThisType> SubDomainGrid;


  //! The type of the iterators over the codim 1 interface between two subdomains on the leaf view.
  typedef typename Traits::LeafSubDomainInterfaceIterator LeafSubDomainInterfaceIterator;

  //! The type of the iterators over the codim 1 interface between two subdomains on a level view.
  typedef typename Traits::LevelSubDomainInterfaceIterator LevelSubDomainInterfaceIterator;

  //! The type of the iterators over the codim 1 interfaces between all subdomains on the leaf view.
  typedef typename Traits::LeafAllSubDomainInterfacesIterator LeafAllSubDomainInterfacesIterator;

  //! The type of the iterators over the codim 1 interfaces between all subdomains on a level view.
  typedef typename Traits::LevelAllSubDomainInterfacesIterator LevelAllSubDomainInterfacesIterator;

  /** @name Constructors */
  /*@{*/
  //! Constructs a new MultiDomainGrid from the given host grid.
  /**
   *
   * \param hostGrid                the host grid that will be wrapped by the MultiDomainGrid
   * \param supportLevelIndexSets   flag indicating support for level index sets on subdomains
   */
  explicit MultiDomainGrid(const std::shared_ptr<HostGrid>& hostGrid, bool supportLevelIndexSets = true) :
    _hostGridPtr(hostGrid),
    _hostGrid(*_hostGridPtr),
    _traits(),
    _leafIndexSet(*this,_hostGrid.leafGridView()),
    _globalIdSet(*this),
    _localIdSet(*this),
    _state(stateFixed),
    _adaptState(stateFixed),
    _supportLevelIndexSets(supportLevelIndexSets),
    _maxAssignedSubDomainIndex(0)
  {
    updateIndexSets();
  }

  //! Constructs a new MultiDomainGrid from the given host grid.
  /**
   *
   * \param hostGrid                the host grid that will be wrapped by the MultiDomainGrid
   * \param traits                  an instance of the grid traits, which might contain runtime information
   * \param supportLevelIndexSets   flag indicating support for level index sets on subdomains
   */
  explicit MultiDomainGrid(const std::shared_ptr<HostGrid>& hostGrid, const MDGridTraitsType& traits, bool supportLevelIndexSets = true) :
    _hostGridPtr(hostGrid),
    _hostGrid(*_hostGridPtr),
    _traits(traits),
    _leafIndexSet(*this,_hostGrid.leafGridView()),
    _globalIdSet(*this),
    _localIdSet(*this),
    _state(stateFixed),
    _adaptState(stateFixed),
    _supportLevelIndexSets(supportLevelIndexSets),
    _maxAssignedSubDomainIndex(0)
  {
    updateIndexSets();
  }


  /** @name Constructors */
  /*@{*/
  //! Constructs a new MultiDomainGrid from the given host grid.
  /**
   *
   * \param hostGrid                the host grid that will be wrapped by the MultiDomainGrid
   * \param supportLevelIndexSets   flag indicating support for level index sets on subdomains
   */
  explicit MultiDomainGrid(HostGrid& hostGrid, bool supportLevelIndexSets = true)
    : MultiDomainGrid(stackobject_to_shared_ptr(hostGrid), supportLevelIndexSets)
  {}

  //! Constructs a new MultiDomainGrid from the given host grid.
  /**
   *
   * \param hostGrid                the host grid that will be wrapped by the MultiDomainGrid
   * \param traits                  an instance of the grid traits, which might contain runtime information
   * \param supportLevelIndexSets   flag indicating support for level index sets on subdomains
   */
  explicit MultiDomainGrid(HostGrid& hostGrid, const MDGridTraitsType& traits, bool supportLevelIndexSets = true)
    : MultiDomainGrid{stackobject_to_shared_ptr(hostGrid), traits, supportLevelIndexSets}
  {}

  /*@}*/

  /** @name Dune grid interface methods */
  /*@{*/

  template<typename EntitySeed>
  typename Traits::template Codim<EntitySeed::codimension>::Entity
  entity(const EntitySeed& entitySeed) const
  {
    return {EntityWrapper<EntitySeed::codimension,dimension,const GridImp>(_hostGrid.entity(entitySeed.hostEntitySeed()))};
  }

  //! The current maximum level of the grid.
  int maxLevel() const {
    return _hostGrid.maxLevel();
  }

  template<int codim>
  typename Traits::template Codim<codim>::LevelIterator lbegin(int level) const {
    return {
      IteratorWrapper<
        typename HostGrid::LevelGridView,
        codim,
        All_Partition,
        const GridImp
        >(_hostGrid.levelGridView(level).template begin<codim>())
    };
  }

  template<int codim>
  typename Traits::template Codim<codim>::LevelIterator lend(int level) const {
    return {
      IteratorWrapper<
        typename HostGrid::LevelGridView,
        codim,
        All_Partition,
        const GridImp
        >(_hostGrid.levelGridView(level).template end<codim>())
    };
  }

  template<int codim, PartitionIteratorType pitype>
  typename Traits::template Codim<codim>::template Partition<pitype>::LevelIterator lbegin(int level) const {
    return {
      IteratorWrapper<
        typename HostGrid::LevelGridView,
        codim,
        pitype,
        const GridImp
        >(_hostGrid.levelGridView(level).template begin<codim,pitype>())
    };
  }

  template<int codim, PartitionIteratorType pitype>
  typename Traits::template Codim<codim>::template Partition<pitype>::LevelIterator lend(int level) const {
    return {
      IteratorWrapper<
        typename HostGrid::LevelGridView,
        codim,
        pitype,
        const GridImp
        >(_hostGrid.levelGridView(level).template end<codim,pitype>())
    };
  }

  template<int codim>
  typename Traits::template Codim<codim>::LeafIterator leafbegin() const {
    return {
      IteratorWrapper<
        typename HostGrid::LeafGridView,
        codim,
        All_Partition,
        const GridImp
        >(_hostGrid.leafGridView().template begin<codim>())
    };
  }

  template<int codim>
  typename Traits::template Codim<codim>::LeafIterator leafend() const {
    return {
      IteratorWrapper<
        typename HostGrid::LeafGridView,
        codim,
        All_Partition,
        const GridImp
        >(_hostGrid.leafGridView().template end<codim>())
    };
  }

  template<int codim, PartitionIteratorType pitype>
  typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator leafbegin() const {
    return {
      IteratorWrapper<
        typename HostGrid::LeafGridView,
        codim,
        pitype,
        const GridImp
        >(_hostGrid.leafGridView().template begin<codim,pitype>())
    };
  }

  template<int codim, PartitionIteratorType pitype>
  typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator leafend() const {
    return {
      IteratorWrapper<
        typename HostGrid::LeafGridView,
        codim,
        pitype,
        const GridImp
        >(_hostGrid.leafGridView().template end<codim,pitype>())
    };
  }
  /*@}*/

  /** @name Methods for iterating over subdomain interfaces */
  /*@{*/
  //! Returns an iterator over the leaf interface of two subdomains.
  /**
   * The resulting iterator will visit all cell intersections that are part of both subdomains.
   *
   * \attention The iterator assumes the two subdomains to be non-overlapping! If there is an overlap,
   * some intersections will be iterated over twice!
   *
   * \param subDomain1 the first subdomain
   * \param subDomain2 the second subdomain
   */
  LeafSubDomainInterfaceIterator leafSubDomainInterfaceBegin(SubDomainIndex subDomain1, SubDomainIndex subDomain2) const {
    return LeafSubDomainInterfaceIterator(*this,subDomain1,subDomain2);
  }

  //! Returns the corresponding end iterator for leafSubDomainInterfaceBegin().
  LeafSubDomainInterfaceIterator leafSubDomainInterfaceEnd(SubDomainIndex subDomain1, SubDomainIndex subDomain2) const {
    return LeafSubDomainInterfaceIterator(*this,subDomain1,subDomain2,true);
  }

  //! Returns an iterator over the interface of two subdomains at the given level.
  /**
   * The resulting iterator will visit all cell intersections that are part of both subdomains.
   *
   * \attention The iterator assumes the two subdomains to be non-overlapping! If there is an overlap,
   * some intersections will be iterated over twice!
   *
   * \param subDomain1 the first subdomain
   * \param subDomain2 the second subdomain
   * \param level      the grid level over which to iterate
   */
  LevelSubDomainInterfaceIterator levelSubDomainInterfaceBegin(SubDomainIndex subDomain1, SubDomainIndex subDomain2, int level) const {
    return LevelSubDomainInterfaceIterator(*this,subDomain1,subDomain2,level);
  }

  //! Returns the corresponding end iterator for levelSubDomainInterfaceBegin().
  /**
   * \param level the grid level to be iterated over.
   */
  LevelSubDomainInterfaceIterator levelSubDomainInterfaceEnd(SubDomainIndex subDomain1, SubDomainIndex subDomain2, int level) const {
    return LevelSubDomainInterfaceIterator(*this,subDomain1,subDomain2,level,true);
  }

  //! Returns an iterator over all subdomain interfaces on the leaf view.
  /**
   * This method returns an iterator that will visit all pairwise surface interfaces
   * between subdomains on the leaf view. In particular, given to adjacent cells
   * \f$e_1\f$ and \f$e_2\f$ and two subdomains \f$s_1\f$ and \f$s_2\f$, this iterator
   * will visit the intersection between \f$e_1\f$ and \f$e_2\f$ iff all of the following hold:
   * \f{eqnarray*} e_1 \in s_1,\ e_1 \not\in s_2,\\ e_2 \not\in s_1,\ e_1 \in s_2.\f}
   * In essence, the two subdomains have to be locally disjoint on \f$e_1\f$ and \f$e_2\f$.
   *
   * The iterator will only traverse the host grid once for visiting all subdomain interfaces.
   * Incrementing the iterator might thus result in an iterator pointing to the same grid
   * intersection, but to a different pair of subdomains. The subdomains pointed to by the
   * iterator can be retrieved by calling LeafAllSubDomainInterfacesIterator::subDomain1()
   * and LeafAllSubDomainInterfacesIterator::subDomain2(), respectively.
   */
  LeafAllSubDomainInterfacesIterator leafAllSubDomainInterfacesBegin() const {
    return LeafAllSubDomainInterfacesIterator(*this);
  }

  //! Returns the corresponding end iterator for leafAllSubDomainInterfacesBegin().
  LeafAllSubDomainInterfacesIterator leafAllSubDomainInterfacesEnd() const {
    return LeafAllSubDomainInterfacesIterator(*this,true);
  }

  //! Returns an iterator over all subdomain interfaces on the requested level view.
  /**
   * This method returns an iterator that will visit all pairwise surface interfaces
   * between subdomains on the requested level view. In particular, given to adjacent cells
   * \f$e_1\f$ and \f$e_2\f$ and two subdomains \f$s_1\f$ and \f$s_2\f$, this iterator
   * will visit the intersection between \f$e_1\f$ and \f$e_2\f$ iff all of the following hold:
   * \f{eqnarray*} e_1 \in s_1,\ e_1 \not\in s_2,\\ e_2 \not\in s_1,\ e_1 \in s_2.\f}
   * In essence, the two subdomains have to be locally disjoint on \f$e_1\f$ and \f$e_2\f$.
   *
   * The iterator will only traverse the host grid once for visiting all subdomain interfaces.
   * Incrementing the iterator might thus result in an iterator pointing to the same grid
   * intersection, but to a different pair of subdomains. The subdomains pointed to by the
   * iterator can be retrieved by calling LevelAllSubDomainInterfacesIterator::subDomain1()
   * and LevelAllSubDomainInterfacesIterator::subDomain2(), respectively.
   *
   * \param level the grid level to be iterated over.
   */
  LevelAllSubDomainInterfacesIterator levelAllSubDomainInterfacesBegin(int level) const {
    return LevelAllSubDomainInterfacesIterator(*this,level);
  }

  //! Returns the corresponding end iterator for levelAllSubDomainInterfacesBegin().
  /**
   * \param level the grid level to be iterated over.
   */
  LevelAllSubDomainInterfacesIterator levelAllSubDomainInterfacesEnd(int level) const {
    return LevelAllSubDomainInterfacesIterator(*this,level,true);
  }
  /*@}*/

  /** @name Dune grid interface methods */
  /*@{*/
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
    if (!_supportLevelIndexSets) {
      DUNE_THROW(GridError,"level index set support not enabled for this grid");
    }
    assert(level <= maxLevel());
    return *_levelIndexSets[level];
  }

  const typename Traits::LeafIndexSet& leafIndexSet() const {
    return _leafIndexSet;
  }

  void globalRefine(int refCount) {
    saveMultiDomainState();
    _hostGrid.globalRefine(refCount);
    updateIndexSets();
    restoreMultiDomainState();
  }

  bool mark(int refCount, const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateFixed);
    return _hostGrid.mark(refCount, hostEntity(e));
  }

  int getMark(const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateFixed);
    return _hostGrid.getMark(hostEntity(e));
  }

  bool preAdapt() {
    assert(_state == stateFixed && _adaptState == stateFixed);
    _adaptState = statePreAdapt;
    bool result = _hostGrid.preAdapt();
    return result;
  }

  bool adapt() {
    assert(_state == stateFixed && _adaptState == statePreAdapt);
    _adaptState = statePostAdapt;
    saveMultiDomainState();
    bool result = _hostGrid.adapt();
    updateIndexSets();
    restoreMultiDomainState();
    return result;
  }

  void postAdapt() {
    assert(_state == stateFixed && _adaptState == statePostAdapt);
    _adaptState = stateFixed;
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

  const typename Traits::Communication& comm() const {
    return _hostGrid.comm();
  }

  template<typename DataHandleImp, typename DataTypeImp>
  void communicate (CommDataHandleIF<DataHandleImp,DataTypeImp> &data,
                    InterfaceType iftype,
                    CommunicationDirection dir,
                    int level) const
  {
    DataHandleWrapper<CommDataHandleIF<DataHandleImp,DataTypeImp> > datahandle(data,*this);
    _hostGrid.levelGridView(level).communicate(datahandle,iftype,dir);
  }

  template<typename DataHandleImp, typename DataTypeImp>
  void communicate (CommDataHandleIF<DataHandleImp,DataTypeImp> &data,
                    InterfaceType iftype,
                    CommunicationDirection dir) const
  {
    DataHandleWrapper<CommDataHandleIF<DataHandleImp,DataTypeImp> > datahandle(data,*this);
    _hostGrid.leafGridView().communicate(datahandle,iftype,dir);
  }

  template<typename DataHandle>
  bool loadBalance(DataHandle& dataHandle)
  {
    typedef typename MultiDomainGrid::LeafGridView GV;
    GV gv = this->leafGridView();
    typedef typename GV::template Codim<0>::Iterator Iterator;
    typedef typename GV::template Codim<0>::Entity Entity;
    typedef typename MDGridTraits::template Codim<0>::SubDomainSet SubDomainSet;
    for (Iterator it = gv.template begin<0>(); it != gv.template end<0>(); ++it) {
      const Entity& e = *it;
      const SubDomainSet& subDomains = gv.indexSet().subDomains(e);
      _loadBalanceStateMap[globalIdSet().id(e)] = subDomains;
    }

    LoadBalancingDataHandle<DataHandle> dataHandleWrapper(*this,dataHandle);
    if (!_hostGrid.loadBalance(dataHandleWrapper))
      return false;

    this->startSubDomainMarking();

    for (Iterator it = gv.template begin<0>(); it != gv.template end<0>(); ++it) {
      _leafIndexSet.addToSubDomains(_loadBalanceStateMap[globalIdSet().id(*it)],*it);
    }

    this->preUpdateSubDomains();
    this->updateSubDomains();
    this->postUpdateSubDomains();

    _loadBalanceStateMap.clear();

    return true;
  }

  bool loadBalance()
  {
    EmptyDataHandle emptyDataHandle;
    return loadBalance(emptyDataHandle);
  }

  size_t numBoundarySegments() const
  {
    return _hostGrid.numBoundarySegments();
  }

  /*@}*/

  /** @name Subdomain creation- and adaptation methods */
  /*@{*/
  //! Prepares the grid for (re-)assigning cells to subdomains.
  /**
   * After calling this method, it becomes possible to invoke the various methods
   * for cell assignment to subdomains. When you are done marking, call preUpdateSubDomains().
   *
   * IMPORTANT: Reassigning subdomains and grid adaptation are mutually exclusive,
   * it is not possibly to do both at the same time. This restriction is enforced
   * by the grid.
   */
  void startSubDomainMarking() {
    assert(_state == stateFixed && _adaptState == stateFixed);
    _tmpLeafIndexSet.reset(new LeafIndexSetImp(_leafIndexSet));
    _tmpLeafIndexSet->reset(false);
    _state = stateMarking;
  }

  //! Calculates the new subdomain layout, but does not update the current subdomains yet.
  /**
   * After calling this method, you can query the grid for the changes that will occur when the
   * new subdomain layout becomes active. This includes the possibility to obtain the new indices
   * entities will be assigned in the modified subdomains.
   *
   * To switch the grid over to the new layout, call updateSubDomains().
   */
  void preUpdateSubDomains() {
    assert(_state == stateMarking && _adaptState == stateFixed);
    if (_supportLevelIndexSets) {
      for (int l = 0; l <= maxLevel(); ++l) {
        _tmpLevelIndexSets.push_back(std::make_shared<LevelIndexSetImp>(*this,_hostGrid.levelGridView(l)));
      }
    }
    _tmpLeafIndexSet->update(_tmpLevelIndexSets,true);
    _state = statePreUpdate;
  }

  //! Switches the subdomain layout over to the new layout.
  /**
   *
   */
  void updateSubDomains() {
    assert(_state == statePreUpdate && _adaptState == stateFixed);
    _leafIndexSet.swap(*_tmpLeafIndexSet);
    if (_supportLevelIndexSets) {
      for (int l = 0; l <= maxLevel(); ++l) {
        _levelIndexSets[l]->swap(*_tmpLevelIndexSets[l]);
      }
    }
    _state = statePostUpdate;
  }

  //! clears the saved state of the subdomain layout that was active before the last call to updateSubDomains().
  void postUpdateSubDomains() {
    assert(_state == statePostUpdate && _adaptState == stateFixed);
    _tmpLevelIndexSets.clear();
    _tmpLeafIndexSet.reset(nullptr);
    _state = stateFixed;
  }

  //! Adds the given leaf entity to the specified subdomain.
  void addToSubDomain(SubDomainIndex subDomain, const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateMarking);
    assert(e.isLeaf());
    assert(e.partitionType() == Dune::InteriorEntity);
    _maxAssignedSubDomainIndex = std::max(_maxAssignedSubDomainIndex,subDomain);
    _tmpLeafIndexSet->addToSubDomain(subDomain,e);
  }

  //! Removes the given leaf entity from the specified subdomain.
  void removeFromSubDomain(SubDomainIndex subDomain, const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateMarking);
    assert(e.isLeaf());
    assert(e.partitionType() == Dune::InteriorEntity);
    _tmpLeafIndexSet->removeFromSubDomain(subDomain,e);
  }

  //! Assigns the given leaf entity to the specified subdomain, clearing any previous subdomain assignments.
  void assignToSubDomain(SubDomainIndex subDomain, const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateMarking);
    assert(e.isLeaf());
    assert(e.partitionType() == Dune::InteriorEntity);
    _maxAssignedSubDomainIndex = std::max(_maxAssignedSubDomainIndex,subDomain);
    _tmpLeafIndexSet->assignToSubDomain(subDomain,e);
  }

  //! Removes the given leaf entity from all subdomains it currently belongs to.
  void removeFromAllSubDomains(const typename Traits::template Codim<0>::Entity& e) {
    assert(_state == stateMarking);
    assert(e.isLeaf());
    assert(e.partitionType() == Dune::InteriorEntity);
    _tmpLeafIndexSet->removeFromAllSubDomains(e);
  }
  /*@}*/

  /** @name Access to the subdomain grids */
  /*@{*/
  //! Returns a reference to the SubDomainGrid associated with the given subdomain.
  const SubDomainGrid& subDomain(SubDomainIndex subDomain) const {
    std::shared_ptr<SubDomainGrid>& subGridPointer = _subDomainGrids[subDomain];
    if (!subGridPointer) {
      subGridPointer.reset(new SubDomainGrid(const_cast<MultiDomainGrid&>(*this),subDomain));
    }
    return *subGridPointer;
  }

  //! Returns a reference to the SubDomainGrid associated with the given subdomain.
  SubDomainGrid& subDomain(SubDomainIndex subDomain) {
    std::shared_ptr<SubDomainGrid>& subGridPointer = _subDomainGrids[subDomain];
    if (!subGridPointer) {
      subGridPointer.reset(new SubDomainGrid(*this,subDomain));
    }
    return *subGridPointer;
  }

  //! Returns the largest subdomain index that was ever assigned to a cell in this grid.
  /**
   * This method returns the largest subdomain index that was passed to addToSubDomain() or
   * assignToSubDomain() since this MultiDomainGrid was created. Keep in mind that the subdomain
   * belonging to that index might not contain any entities anymore if all entities have been
   * removed from it at a later point.
   */
  SubDomainIndex maxAssignedSubDomainIndex() const
  {
    return _maxAssignedSubDomainIndex;
  }

  //! Indicates whether this MultiDomainGrid instance supports level index sets on its SubDomainGrids.
  bool supportLevelIndexSets() const {
    return _supportLevelIndexSets;
  }
  /*@}*/

  /** @name Entity conversion methods */
  /*@{*/
  //! Returns a reference to the corresponding host entity.
  /**
   * \warning The returned reference will only be valid as long as the passed-in reference to the
   * MultiDomainGrid entity! If you need a persistent host entity object , copy the returned reference.
   */
  template<typename EntityType>
  static const typename HostEntity<EntityType>::type& hostEntity(const EntityType& e)
  {
    return e.impl().hostEntity();
  }

  template<typename EntityType>
  static const typename MultiDomainEntity<EntityType>::type& multiDomainEntity(const EntityType& e)
  {
    return e.impl().multiDomainEntity();
  }

  template<typename IntersectionType>
  static const auto& multiDomainIntersection(const IntersectionType& is) {
    return SubDomainGrid::multiDomainIntersection(is);
  }
 /*@}*/

  const MDGridTraits& traits() const
  {
    return _traits;
  }

private:

  const HostGrid& hostGrid() const
  {
    return _hostGrid;
  }

  HostGrid& hostGrid()
  {
    return _hostGrid;
  }

  std::shared_ptr<HostGrid> _hostGridPtr;
  HostGrid& _hostGrid;
  const MDGridTraitsType _traits;

  std::vector<std::shared_ptr<LevelIndexSetImp> > _levelIndexSets;
  LeafIndexSetImp _leafIndexSet;

  std::vector<std::shared_ptr<LevelIndexSetImp> > _tmpLevelIndexSets;
  std::unique_ptr<LeafIndexSetImp> _tmpLeafIndexSet;

  GlobalIdSetImp _globalIdSet;
  LocalIdSetImp _localIdSet;

  State _state;
  State _adaptState;
  const bool _supportLevelIndexSets;

  mutable std::map<SubDomainIndex,std::shared_ptr<SubDomainGrid> > _subDomainGrids;
  SubDomainIndex _maxAssignedSubDomainIndex;

  AdaptationStateMap _adaptationStateMap;
  LoadBalanceStateMap _loadBalanceStateMap;

  void updateIndexSets() {
    // make sure we have enough LevelIndexSets
    if (_supportLevelIndexSets) {
      while (static_cast<int>(_levelIndexSets.size()) <= maxLevel()) {
        _levelIndexSets.push_back(std::make_shared<LevelIndexSetImp>(*this,_hostGrid.levelGridView(_levelIndexSets.size())));
      }
      // and make sure we don't have too many...
      if (static_cast<int>(_levelIndexSets.size()) > maxLevel() + 1)
        {
          _levelIndexSets.resize(maxLevel() + 1);
        }
    }

    _leafIndexSet.reset(true);
    _leafIndexSet.update(_levelIndexSets,true);

    _globalIdSet.update(_hostGrid.globalIdSet());
    _localIdSet.update(_hostGrid.localIdSet());

    // rebuild level index sets on subgrids
    for (auto& subGridPair : _subDomainGrids)
      subGridPair.second->update();
  }

  void saveMultiDomainState() {
    typedef typename ThisType::LeafGridView GV;
    GV gv = this->leafGridView();
    typedef typename GV::template Codim<0>::Entity Entity;
    typedef typename MDGridTraits::template Codim<0>::SubDomainSet SubDomainSet;
    for (const auto& e : elements(gv)) {
      const SubDomainSet& subDomains = gv.indexSet().subDomains(e);
      _adaptationStateMap[localIdSet().id(e)] = subDomains;
      Entity he(e);
      while (he.mightVanish()) {
        he = he.father();
        typename Traits::LocalIdSet::IdType id = localIdSet().id(he);
        // if the entity has not been added to the set, create a new entry
        // (entity returns false and does not change the map if there is already an entry for "id")
        if (!_adaptationStateMap.insert(typename AdaptationStateMap::value_type(id,subDomains)).second) {
          // otherwise add the leaf entity's subdomains to the existing set of subdomains
          _adaptationStateMap[id].addAll(subDomains);
        }
      }
    }
  }

  void restoreMultiDomainState() {
    typedef typename ThisType::LeafGridView GV;
    GV gv = this->leafGridView();
    typedef typename GV::template Codim<0>::Entity Entity;
    for (const auto& e : elements(gv)) {
      Entity he(e);
      // First try to exploit the information in the underlying grid
      while (he.isNew()) {
        he = he.father();
      }
      // This might not work, as there are no isNew() marks for globalrefine()
      // We thus have to look up the former leaf entity in our adaptation map
      typename AdaptationStateMap::iterator asmit = _adaptationStateMap.find(localIdSet().id(he));
      while(asmit == _adaptationStateMap.end()) {
        he = he.father();
        asmit = _adaptationStateMap.find(localIdSet().id(he));
      }
      _leafIndexSet.addToSubDomains(asmit->second, e);
    }
    _leafIndexSet.update(_levelIndexSets,false);
    _adaptationStateMap.clear();
  }

  template<typename GridView, typename HostGridView>
  static typename GridView::IntersectionIterator multiDomainIntersectionIterator(typename HostGridView::IntersectionIterator iit) {
    typedef decltype(std::declval<typename GridView::IntersectionIterator>().impl()) Implementation;
    return Implementation(iit);
  }

public:

  template<typename Entity>
  typename Traits::template Codim<Entity::codimension>::Entity wrapHostEntity(const Entity& e) const {
    return wrapHostEntity<Entity::codimension>(e);
  }

  template<int codim>
  typename Traits::template Codim<codim>::Entity wrapHostEntity(const typename HostGrid::template Codim<codim>::Entity& e) const
  {
    return {EntityWrapper<codim,dimension,const GridImp>(e)};
  }

private:


  template<typename Impl>
  struct DataHandleWrapper
    : public Dune::CommDataHandleIF<DataHandleWrapper<Impl>,
                                    typename Impl::DataType
                                    >
  {

    bool contains(int dim, int codim) const
    {
      return _impl.contains(dim,codim); // TODO: check if codim supported
    }

    bool fixedSize(int dim, int codim) const
    {
      return _impl.fixedSize(dim,codim);
    }

    template<typename Entity>
    std::size_t size(const Entity& e) const
    {
      return _impl.size(_grid.wrapHostEntity(e));
    }

    template<typename MessageBufferImp, typename Entity>
    void gather(MessageBufferImp& buf, const Entity& e) const
    {
      _impl.gather(buf,_grid.wrapHostEntity(e));
    }

    template<typename MessageBufferImp, typename Entity>
    void scatter(MessageBufferImp& buf, const Entity& e, std::size_t n)
    {
      _impl.scatter(buf,_grid.wrapHostEntity(e),n);
    }

    DataHandleWrapper(Impl& impl, const MultiDomainGrid<HostGrid,MDGridTraitsType>& grid)
      : _impl(impl)
      , _grid(grid)
    {}

    Impl& _impl;
    const MultiDomainGrid<HostGrid,MDGridTraitsType>& _grid;

  };


  template<typename WrappedDataHandle>
  struct LoadBalancingDataHandle
    : public Dune::CommDataHandleIF<LoadBalancingDataHandle<WrappedDataHandle>,
                                    typename WrappedDataHandle::DataType
                                    >
  {

    union Data
    {
      SubDomainIndex data;
      typename WrappedDataHandle::DataType buffer;
    };

    static_assert(sizeof(Data) >= sizeof(SubDomainIndex),
                  "During load balancing, the data type has to be large enough to contain MultiDomaingrid::SubDomainIndex");

    bool contains(int dim, int codim) const
    {
      return (codim == 0)
        || _wrappedDataHandle.contains(dim,codim);
    }

    bool fixedSize(int dim, int codim) const
    {
      return false;
    }

    template<typename Entity>
    std::size_t size(const Entity& e) const
    {
      if (_grid.leafGridView().indexSet().contains(e) && e.partitionType() == Dune::InteriorEntity)
        return _grid.leafGridView().indexSet().subDomains(e).size() + 1 + _wrappedDataHandle.size(e);
      else
        return _wrappedDataHandle.size(e);
    }

    template<typename MessageBufferImp, typename Entity>
    void gather(MessageBufferImp& buf, const Entity& e) const
    {
      assert(Entity::codimension == 0);
      if (e.partitionType() == Dune::InteriorEntity && _grid.leafGridView().indexSet().contains(e))
        {
          const auto& subDomains = _grid.leafGridView().indexSet().subDomains(e);
          Data size = { subDomains.size() };
          buf.write(size.buffer);
          for (auto& sub_domain : subDomains)
            {
              Data subDomain = { sub_domain };
              buf.write(subDomain.buffer);
            }
        }
      _wrappedDataHandle.gather(buf,e);
    }

    template<typename MessageBufferImp, typename Entity>
    void scatter(MessageBufferImp& buf, const Entity& e, std::size_t n)
    {
      if (e.partitionType() != Dune::InteriorEntity && _grid.leafGridView().indexSet().contains(e))
        {
          Data subDomains = { 0 };
          buf.read(subDomains.buffer);
          for (int i = 0; i < subDomains.data; ++i)
            {
              Data subDomain = { 0 };
              buf.read(subDomain.buffer);
              _grid._loadBalanceStateMap[_grid.globalIdSet().id(multiDomainEntity(e))].add(subDomain.data);
            }
          _wrappedDataHandle.scatter(buf,e,n - (subDomains.data + 1));
        }
      else
        _wrappedDataHandle.scatter(buf,e,n);
    }

    LoadBalancingDataHandle(MultiDomainGrid& grid, WrappedDataHandle& wrappedDataHandle)
      : _grid(grid)
      , _wrappedDataHandle(wrappedDataHandle)
    {}

    MultiDomainGrid& _grid;
    WrappedDataHandle& _wrappedDataHandle;

  };

  struct EmptyDataHandle
    : public Dune::CommDataHandleIF<EmptyDataHandle,
                                    SubDomainIndex
                                    >
  {

    bool contains(int dim, int codim) const
    {
      return false;
    }

    bool fixedSize(int dim, int codim) const
    {
      return true;
    }

    template<typename Entity>
    std::size_t size(const Entity& e) const
    {
      return 0;
    }

    template<typename MessageBufferImp, typename Entity>
    void gather(MessageBufferImp& buf, const Entity& e) const
    {
    }

    template<typename MessageBufferImp, typename Entity>
    void scatter(MessageBufferImp& buf, const Entity& e, std::size_t n)
    {
    }

  };

};

enum MultiDomainGridType { multiDomainGrid, subDomainGrid, other };

template<typename T>
struct GridType {
  static const MultiDomainGridType v = other;
};

template<class HostGrid, typename MDGridTraits>
struct GridType<MultiDomainGrid<HostGrid,MDGridTraits> > {
  static const MultiDomainGridType v = multiDomainGrid;
};

template<class MDGrid>
struct GridType<subdomain::SubDomainGrid<MDGrid> > {
  static const MultiDomainGridType v = subDomainGrid;
};

} // namespace mdgrid

} // namespace Dune

#endif // DUNE_MULTIDOMAINGRID_MULTIDOMAINGRID_HH
