#ifndef __CXXMPH_MPH_MAP_H__
#define __CXXMPH_MPH_MAP_H__
// Implementation of the unordered associative mapping interface using a
// minimal perfect hash function.
//
// Since these are header-mostly libraries, make sure you compile your code
// with -DNDEBUG and -O3. The code requires a modern C++11 compiler.
//
// The container comes in 3 flavors, all in the cxxmph namespace and drop-in
// replacement for the popular classes with the same names.
// * dense_hash_map
//    -> fast, uses more memory, 2.93 bits per bucket, ~50% occupation
// * unordered_map (aliases:  hash_map, mph_map)
//    -> middle ground, uses 2.93 bits per bucket, ~81% occupation
// * sparse_hash_map -> slower, uses 3.6 bits per bucket
//    -> less fast, uses 3.6 bits per bucket, 100% occupation
//
// Those classes are not necessarily faster than their existing counterparts.
// Benchmark your code before using it. The larger the key, the larger the
// number of elements inserted, and the bigger the number of failed searches,
// the more likely those classes will outperform existing code.
//
// For large sets of urls (>100k), which are a somewhat expensive to compare, I
// found those class to be about 10%-50% faster than unordered_map.

#include <algorithm>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>  // for std::pair

#include "string_util.h"
#include "hollow_iterator.h"
#include "mph_bits.h"
#include "mph_index.h"
#include "seeded_hash.h"

namespace cxxmph {

using std::pair;
using std::make_pair;
using std::vector;

// Save on repetitive typing.
#define MPH_MAP_TMPL_SPEC  \
    template <bool minimal, bool square, \
    class Key, class Data, class HashFcn, class EqualKey, class Alloc>
#define MPH_MAP_CLASS_SPEC mph_map_base<minimal, square, Key, Data, HashFcn, EqualKey, Alloc>
#define MPH_MAP_METHOD_DECL(r, m) MPH_MAP_TMPL_SPEC typename MPH_MAP_CLASS_SPEC::r MPH_MAP_CLASS_SPEC::m
#define MPH_MAP_INLINE_METHOD_DECL(r, m) MPH_MAP_TMPL_SPEC inline typename MPH_MAP_CLASS_SPEC::r MPH_MAP_CLASS_SPEC::m

template <bool minimal, bool square, class Key, class Data, class HashFcn = std::hash<Key>, class EqualKey = std::equal_to<Key>, class Alloc = std::allocator<Data> >
class mph_map_base {
 public:
  typedef Key key_type;
  typedef Data data_type;
  typedef pair<Key, Data> value_type;
  typedef HashFcn hasher;
  typedef EqualKey key_equal;

  typedef typename vector<value_type>::pointer pointer;
  typedef typename vector<value_type>::reference reference;
  typedef typename vector<value_type>::const_reference const_reference;
  typedef typename vector<value_type>::size_type size_type;
  typedef typename vector<value_type>::difference_type difference_type;

  typedef is_empty<const vector<value_type>> is_empty_type;
  typedef hollow_iterator_base<typename vector<value_type>::iterator, is_empty_type> iterator;
  typedef hollow_iterator_base<typename vector<value_type>::const_iterator, is_empty_type> const_iterator;

  // For making macros simpler.
  typedef void void_type;
  typedef bool bool_type;
  typedef pair<iterator, bool> insert_return_type;

  mph_map_base();
  ~mph_map_base();

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  size_type size() const;
  bool empty() const;
  void clear();
  void erase(iterator pos);
  void erase(const key_type& k);
  pair<iterator, bool> insert(const value_type& x);
  inline iterator find(const key_type& k);
  inline const_iterator find(const key_type& k) const;
  typedef int32_t my_int32_t;  // help macros
  inline int32_t index(const key_type& k) const;
  data_type& operator[](const key_type &k);
  const data_type& operator[](const key_type &k) const;

  size_type bucket_count() const { return index_.size() + slack_.bucket_count(); }
  void rehash(size_type nbuckets /*ignored*/); 

 protected:  // mimicking STL implementation
  EqualKey equal_;

 private:
   template <typename iterator>
   struct iterator_first : public iterator {
     iterator_first(iterator it) : iterator(it) { }
     const typename iterator::value_type::first_type& operator*() {
      return this->iterator::operator*().first;
     }
   };

   template <typename iterator>
     iterator_first<iterator> make_iterator_first(iterator it) {
     return iterator_first<iterator>(it);
   }

   void pack();
   vector<value_type> values_;
   vector<bool> present_;
   FlexibleMPHIndex<minimal, square, Key, typename seeded_hash<HashFcn>::hash_function> index_;
   // TODO(davi) optimize slack to use hash from index rather than calculate its own
   typedef std::unordered_map<h128, uint32_t, h128::hash32> slack_type;
   slack_type slack_;
   size_type size_;
   typename seeded_hash<HashFcn>::hash_function hasher128_;
};

MPH_MAP_TMPL_SPEC
bool operator==(const MPH_MAP_CLASS_SPEC& lhs, const MPH_MAP_CLASS_SPEC& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

MPH_MAP_TMPL_SPEC MPH_MAP_CLASS_SPEC::mph_map_base() : size_(0) {
  clear();
  pack();
}
MPH_MAP_TMPL_SPEC MPH_MAP_CLASS_SPEC::~mph_map_base() { }

MPH_MAP_METHOD_DECL(insert_return_type, insert)(const value_type& x) {
  auto it = find(x.first);
  auto it_end = end();
  if (it != it_end) return make_pair(it, false);
  bool should_pack = false;
  if (values_.capacity() == values_.size() && values_.size() > 256) {
    should_pack = true;
  }
  values_.push_back(x);
  present_.push_back(true);
  ++size_;
  h128 h = hasher128_.hash128(x.first, 0);
  if (slack_.find(h) != slack_.end()) should_pack = true;  // unavoidable pack
  else slack_.insert(std::make_pair(h, values_.size() - 1));
  if (should_pack) pack();
  it = find(x.first);
  return make_pair(it, true);
}

MPH_MAP_METHOD_DECL(void_type, pack)() {
  // CXXMPH_DEBUGLN("Packing %v values")(values_.size());
  if (values_.empty()) return;
  assert(std::unordered_set<key_type>(make_iterator_first(begin()), make_iterator_first(end())).size() == size());
  bool success = index_.Reset(
      make_iterator_first(begin()),
      make_iterator_first(end()), size_);
  if (!success) { exit(-1); }
  vector<value_type> new_values(index_.size());
  new_values.reserve(new_values.size() * 2);
  vector<bool> new_present(index_.size(), false);
  new_present.reserve(new_present.size() * 2);
  for (iterator it = begin(), it_end = end(); it != it_end; ++it) {
    size_type id = index_.index(it->first);
    assert(id < index_.size());
    assert(id < new_values.size());
    new_values[id] = *it;
    new_present[id] = true;
  }
  // fprintf(stderr, "Collision ratio: %f\n", collisions*1.0/size());
  values_.swap(new_values);
  present_.swap(new_present);
  slack_type().swap(slack_);
}

MPH_MAP_METHOD_DECL(iterator, begin)() { return make_hollow(&values_, &present_, values_.begin()); }
MPH_MAP_METHOD_DECL(iterator, end)() { return make_solid(&values_, &present_, values_.end()); }
MPH_MAP_METHOD_DECL(const_iterator, begin)() const { return make_hollow(&values_, &present_, values_.begin()); }
MPH_MAP_METHOD_DECL(const_iterator, end)() const { return make_solid(&values_, &present_, values_.end()); }
MPH_MAP_METHOD_DECL(bool_type, empty)() const { return size_ == 0; }
MPH_MAP_METHOD_DECL(size_type, size)() const { return size_; }

MPH_MAP_METHOD_DECL(void_type, clear)() {
  values_.clear();
  present_.clear();
  slack_.clear();
  index_.clear();
  size_ = 0;
}

MPH_MAP_METHOD_DECL(void_type, erase)(iterator pos) {
  assert(pos.it_ - values_.begin() < present_.size());
  assert(present_[pos.it_ - values_.begin()]);
  present_[pos.it_ - values_.begin()] = false;
  *pos = value_type();
  --size_;
}
MPH_MAP_METHOD_DECL(void_type, erase)(const key_type& k) {
  iterator it = find(k);
  if (it == end()) return;
  erase(it);
}

MPH_MAP_INLINE_METHOD_DECL(const_iterator, find)(const key_type& k) const {
  auto idx = index(k);
  typename vector<value_type>::const_iterator vit = values_.begin() + idx;
  if (idx == -1 || !equal_(vit->first, k)) return end();
  return make_solid(&values_, &present_, vit);;
}

MPH_MAP_INLINE_METHOD_DECL(iterator, find)(const key_type& k) {
  auto idx = index(k);
  typename vector<value_type>::iterator vit = values_.begin() + idx;
  if (idx == -1 || !equal_(vit->first, k)) return end();
  return make_solid(&values_, &present_, vit);;
}

MPH_MAP_INLINE_METHOD_DECL(my_int32_t, index)(const key_type& k) const {
  if (__builtin_expect(!slack_.empty(), 0)) {
     auto sit = slack_.find(hasher128_.hash128(k, 0));
     if (sit != slack_.end()) return sit->second;
  }
  if (__builtin_expect(index_.size(), 1)) {
    auto id = index_.index(k);
    if (__builtin_expect(present_[id], true)) return id;
  }
  return -1;
}

MPH_MAP_METHOD_DECL(data_type&, operator[])(const key_type& k) {
  return insert(make_pair(k, data_type())).first->second;
}
MPH_MAP_METHOD_DECL(void_type, rehash)(size_type nbuckets) {
  pack();
  vector<value_type>(values_.begin(), values_.end()).swap(values_);
  vector<bool>(present_.begin(), present_.end()).swap(present_);
  slack_type().swap(slack_);
}

#define MPH_MAP_PREAMBLE template <class Key, class Data,\
     class HashFcn = std::hash<Key>, class EqualKey = std::equal_to<Key>,\
     class Alloc = std::allocator<Data> >

MPH_MAP_PREAMBLE class mph_map : public mph_map_base<
     false, false, Key, Data, HashFcn, EqualKey, Alloc> {};
MPH_MAP_PREAMBLE class unordered_map : public mph_map_base<
     false, false, Key, Data, HashFcn, EqualKey, Alloc> {};
MPH_MAP_PREAMBLE class hash_map : public mph_map_base<
     false, false, Key, Data, HashFcn, EqualKey, Alloc> {};

MPH_MAP_PREAMBLE class dense_hash_map : public mph_map_base<
     false, true, Key, Data, HashFcn, EqualKey, Alloc> {};
MPH_MAP_PREAMBLE class sparse_hash_map : public mph_map_base<
     true, false, Key, Data, HashFcn, EqualKey, Alloc> {};

}  // namespace cxxmph

#endif  // __CXXMPH_MPH_MAP_H__
