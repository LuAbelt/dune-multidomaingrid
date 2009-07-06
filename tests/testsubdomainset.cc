#include <dune/grid/multidomaingrid/subdomainset.hh>
#include <iostream>
#include <typeinfo>

int main(int argc, char** argv) {
  try {
    typedef Dune::multidomaingrid::IntegralTypeSubDomainSet<53> Set;
    //std::cout << typeid(Set::SetStorage).name() << std::endl;
    std::cout << std::numeric_limits<Set::SetStorage>::digits << std::endl;
    Set set;
    set.add(2);
    set.add(5);
    set.add(50);
    set.add(25);
    set.add(0);
    for (Set::Iterator it = set.begin(); it != set.end(); ++it)
      std::cout << *it << " ";
    std::cout << "size=" << set.size() << std::endl;
    set.set(8);
    for (Set::Iterator it = set.begin(); it != set.end(); ++it)
      std::cout << *it << " ";
    std::cout << "size=" << set.size() << std::endl;
    set.clear();
    for (Set::Iterator it = set.begin(); it != set.end(); ++it)
      std::cout << *it << " ";
    std::cout << "size=" << set.size() << std::endl;
    return 0;
  } catch (Dune::Exception& e) {
    std::cerr << e << std::endl;
    return 1;
  }
}