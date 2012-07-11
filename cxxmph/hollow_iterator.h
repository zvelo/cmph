#ifndef __CXXMPH_HOLLOW_ITERATOR_H__
#define __CXXMPH_HOLLOW_ITERATOR_H__

#include <vector>

namespace cxxmph {

using std::vector;

template <typename container_type>
struct is_empty {
 public:
  is_empty() : c_(NULL), p_(NULL) {};
  is_empty(const container_type* c, const vector<bool>* p) : c_(c), p_(p) {};
  bool operator()(typename container_type::const_iterator it) const {
    if (it == c_->end()) return false;
    return !(*p_)[it - c_->begin()];
  }
 private:
  const container_type* c_;
  const vector<bool>* p_;
};

template <typename iterator, typename is_empty>
struct hollow_iterator_base
    : public std::iterator<std::forward_iterator_tag,
                           typename iterator::value_type> {
 public:
  typedef hollow_iterator_base<iterator, is_empty> self_type;
  typedef self_type& self_reference;
  typedef typename iterator::reference reference;
  typedef typename iterator::pointer pointer;
  inline hollow_iterator_base() : it_(), empty_() { }
  inline hollow_iterator_base(iterator it, is_empty empty, bool solid) : it_(it), empty_(empty) {
    if (!solid) advance();
  }
  // Same as above, assumes solid==true.
  inline hollow_iterator_base(iterator it, is_empty empty) : it_(it), empty_(empty) {}
  inline hollow_iterator_base(const self_type& rhs) { it_ = rhs.it_; empty_ = rhs.empty_; }
  template <typename const_iterator>
  hollow_iterator_base(const hollow_iterator_base<const_iterator, is_empty>& rhs) { it_ = rhs.it_; empty_ = rhs.empty_; }

  reference operator*() { return *it_;  }
  pointer operator->() { return &(*it_); }
  self_reference operator++() { ++it_; advance(); return *this; }
  // self_type operator++() { auto tmp(*this); ++tmp; return tmp; }

  template <typename const_iterator>
  bool operator==(const hollow_iterator_base<const_iterator, is_empty>& rhs) { return rhs.it_ == it_; }
  template <typename const_iterator>
  bool operator!=(const hollow_iterator_base<const_iterator, is_empty>& rhs) { return rhs.it_ != it_; }

  // should be friend
  iterator it_;
  is_empty empty_;

 private:
  void advance() {
    while (empty_(it_)) ++it_;
  }
};

template <typename container_type, typename iterator>
inline auto make_solid(
   container_type* v, const vector<bool>* p, iterator it) ->
       hollow_iterator_base<iterator, is_empty<const container_type>> {
  return hollow_iterator_base<iterator, is_empty<const container_type>>(
      it, is_empty<const container_type>(v, p));
}

template <typename container_type, typename iterator>
inline auto make_hollow(
   container_type* v, const vector<bool>* p, iterator it) ->
       hollow_iterator_base<iterator, is_empty<const container_type>> {
  return hollow_iterator_base<iterator, is_empty<const container_type>>(
      it, is_empty<const container_type>(v, p), false);
}

}  // namespace cxxmph

#endif  // __CXXMPH_HOLLOW_ITERATOR_H__
