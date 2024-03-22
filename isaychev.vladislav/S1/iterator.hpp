#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <stdexcept>
#include <utility>
#include "node.hpp"

namespace isaychev
{
  template < typename T >
  class fwdIterator
  {
    using this_t = fwdIterator< T >;

   public:
    fwdIterator();
    fwdIterator(nodeNS::node_t< T > * pos);

    this_t operator++();
    this_t operator++(int);
    T & operator*();
    T * operator->();
    const T & operator*() const;
    const T * operator->() const;
    bool operator==(const this_t & rhs) const;
    bool operator!=(const this_t & rhs) const;

   private:
    nodeNS::node_t< T > * currNode_;
  };

  template < typename T >
  fwdIterator< T >::fwdIterator():
   currNode_(nullptr)
  {}

  template < typename T >
  fwdIterator< T >::fwdIterator(nodeNS::node_t< T > * pos):
   currNode_(pos)
  {}

  template < typename T >
  fwdIterator< T > fwdIterator< T >::operator++()
  {
    currNode_ = currNode_->next;
    return *this;
  }

  template < typename T >
  fwdIterator< T > fwdIterator< T >::operator++(int)
  {
    this_t res(*this);
    ++(*this);
    return res;
  }

  template < typename T >
  T & fwdIterator< T >::operator*()
  {
    return currNode_->data;
  }

  template < typename T >
  T * fwdIterator< T >::operator->()
  {
    return std::addressof(currNode_->data);
  }

  template < typename T >
  bool fwdIterator< T >::operator==(const this_t & rhs) const
  {
    return (currNode_ == rhs.currNode_);
  }

  template < typename T >
  bool fwdIterator< T >::operator!=(const this_t & rhs) const
  {
    return !(currNode_ == rhs.currNode_);
  }
}

#endif
