// Copyright (c) 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ----
//
// This is just a very thin wrapper over densehashtable.h, just
// like sgi stl's stl_hash_map is a very thin wrapper over
// stl_hashtable.  The major thing we define is operator[], because
// we have a concept of a data_type which stl_hashtable doesn't
// (it only has a key and a value).
//
// NOTE: this is exactly like sparse_hash_map.h, with the word
// "sparse" replaced by "dense", except for the addition of
// set_empty_key().
//
//   YOU MUST CALL SET_EMPTY_KEY() IMMEDIATELY AFTER CONSTRUCTION.
//
// Otherwise your program will die in mysterious ways.  (Note if you
// use the constructor that takes an InputIterator range, you pass in
// the empty key in the constructor, rather than after.  As a result,
// this constructor differs from the standard STL version.)
//
// In other respects, we adhere mostly to the STL semantics for
// hash-map.  One important exception is that insert() may invalidate
// iterators entirely -- STL semantics are that insert() may reorder
// iterators, but they all still refer to something valid in the
// hashtable.  Not so for us.  Likewise, insert() may invalidate
// pointers into the hashtable.  (Whether insert invalidates iterators
// and pointers depends on whether it results in a hashtable resize).
// On the plus side, delete() doesn't invalidate iterators or pointers
// at all, or even change the ordering of elements.
//
// Here are a few "power user" tips:
//
//    1) set_deleted_key():
//         If you want to use erase() you *must* call set_deleted_key(),
//         in addition to set_empty_key(), after construction.
//         The deleted and empty keys must differ.
//
//    2) resize(0):
//         When an item is deleted, its memory isn't freed right
//         away.  This allows you to iterate over a hashtable,
//         and call erase(), without invalidating the iterator.
//         To force the memory to be freed, call resize(0).
//         For tr1 compatibility, this can also be called as rehash(0).
//
//    3) min_load_factor(0.0)
//         Setting the minimum load factor to 0.0 guarantees that
//         the hash table will never shrink.
//
// Roughly speaking:
//   (1) dense_hash_map: fastest, uses the most memory unless entries are small
//   (2) sparse_hash_map: slowest, uses the least memory
//   (3) hash_map / unordered_map (STL): in the middle
//
// Typically I use sparse_hash_map when I care about space and/or when
// I need to save the hashtable on disk.  I use hash_map otherwise.  I
// don't personally use dense_hash_set ever; some people use it for
// small sets with lots of lookups.
//
// - dense_hash_map has, typically, about 78% memory overhead (if your
//   data takes up X bytes, the hash_map uses .78X more bytes in overhead).
// - sparse_hash_map has about 4 bits overhead per entry.
// - sparse_hash_map can be 3-7 times slower than the others for lookup and,
//   especially, inserts.  See time_hash_map.cc for details.
//
// See /usr/(local/)?doc/sparsehash-*/dense_hash_map.html
// for information about how to use this class.

#pragma once

#include <algorithm>   // needed by stl_alloc
#include <functional>  // for equal_to<>, select1st<>, etc
#include <initializer_list> // for initializer_list
#include <memory>      // for alloc
#include <utility>     // for pair<>
#include <tuple>       // forward_as_tuple
#include <type_traits> // for enable_if, is_constructible, etc
#include <Sparsehash/internal/densehashtable.h>  // IWYU pragma: export
#include <Sparsehash/internal/libc_allocator_with_realloc.h>

namespace google {

template <class Key, class T, class HashFcn = std::hash<Key>,
          class EqualKey = std::equal_to<Key>,
          class Alloc = libc_allocator_with_realloc<std::pair<const Key, T>>>
class dense_hash_map {
 private:
  // Apparently select1st is not stl-standard, so we define our own
  struct SelectKey {
    typedef const Key& result_type;
    template <typename Pair>
    const Key& operator()(Pair&& p) const {
      return p.first;
    }
  };
  struct SetKey {
    void operator()(std::pair<const Key, T>* value, const Key& new_key) const {
      using NCKey = typename std::remove_cv<Key>::type;
      *const_cast<NCKey*>(&value->first) = new_key;

      // It would be nice to clear the rest of value here as well, in
      // case it's taking up a lot of memory.  We do this by clearing
      // the value.  This assumes T has a zero-arg constructor!
      value->second = T();
    }
    void operator()(std::pair<const Key, T>* value, const Key& new_key, bool) const {
      new(value) std::pair<const Key, T>(std::piecewise_construct, std::forward_as_tuple(new_key), std::forward_as_tuple());
    }
  };

  // The actual data
  typedef typename sparsehash_internal::key_equal_chosen<HashFcn, EqualKey>::type EqualKeyChosen;
  typedef dense_hashtable<std::pair<const Key, T>, Key, HashFcn, SelectKey,
                          SetKey, EqualKeyChosen, Alloc> ht;
  ht rep;

  static_assert(!sparsehash_internal::has_transparent_key_equal<HashFcn>::value
                || std::is_same<EqualKey, std::equal_to<Key>>::value
                || std::is_same<EqualKey, EqualKeyChosen>::value,
                "Heterogeneous lookup requires key_equal to either be the default container value or the same as the type provided by hash");

 public:
  typedef typename ht::key_type key_type;
  typedef T data_type;
  typedef T mapped_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;
  typedef Alloc allocator_type;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::reference reference;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::iterator iterator;
  typedef typename ht::const_iterator const_iterator;
  typedef typename ht::local_iterator local_iterator;
  typedef typename ht::const_local_iterator const_local_iterator;

  // Iterator functions
  iterator begin() { return rep.begin(); }
  iterator end() { return rep.end(); }
  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }
  const_iterator cbegin() const { return rep.begin(); }
  const_iterator cend() const { return rep.end(); }

  // These come from tr1's unordered_map. For us, a bucket has 0 or 1 elements.
  local_iterator begin(size_type i) { return rep.begin(i); }
  local_iterator end(size_type i) { return rep.end(i); }
  const_local_iterator begin(size_type i) const { return rep.begin(i); }
  const_local_iterator end(size_type i) const { return rep.end(i); }
  const_local_iterator cbegin(size_type i) const { return rep.begin(i); }
  const_local_iterator cend(size_type i) const { return rep.end(i); }

  // Accessor functions
  allocator_type get_allocator() const { return rep.get_allocator(); }
  hasher hash_funct() const { return rep.hash_funct(); }
  hasher hash_function() const { return hash_funct(); }
  key_equal key_eq() const { return rep.key_eq(); }

  // Constructors
  explicit dense_hash_map(size_type expected_max_items_in_table = 0,
                          const hasher& hf = hasher(),
                          const key_equal& eql = key_equal(),
                          const allocator_type& alloc = allocator_type())
      : rep(expected_max_items_in_table, hf, eql, SelectKey(), SetKey(),
            alloc) {}

  template <class InputIterator>
  dense_hash_map(InputIterator f, InputIterator l,
                 const key_type& empty_key_val,
                 size_type expected_max_items_in_table = 0,
                 const hasher& hf = hasher(),
                 const key_equal& eql = key_equal(),
                 const allocator_type& alloc = allocator_type())
      : rep(expected_max_items_in_table, hf, eql, SelectKey(), SetKey(),
            alloc) {
    set_empty_key(empty_key_val);
    rep.insert(f, l);
  }
  // We use the default copy constructor
  // We use the default operator=()
  // We use the default destructor

  void clear() { rep.clear(); }
  // This clears the hash map without resizing it down to the minimum
  // bucket count, but rather keeps the number of buckets constant
  void clear_no_resize() { rep.clear_no_resize(); }
  void swap(dense_hash_map& hs) { rep.swap(hs.rep); }

  // Functions concerning size
  size_type size() const { return rep.size(); }
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }
  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }

  // These are tr1 methods.  bucket() is the bucket the key is or would be in.
  size_type bucket_size(size_type i) const { return rep.bucket_size(i); }
  size_type bucket(const key_type& key) const { return rep.bucket(key); }
  float load_factor() const { return size() * 1.0f / bucket_count(); }
  float max_load_factor() const {
    float shrink, grow;
    rep.get_resizing_parameters(&shrink, &grow);
    return grow;
  }
  void max_load_factor(float new_grow) {
    float shrink, grow;
    rep.get_resizing_parameters(&shrink, &grow);
    rep.set_resizing_parameters(shrink, new_grow);
  }
  // These aren't tr1 methods but perhaps ought to be.
  float min_load_factor() const {
    float shrink, grow;
    rep.get_resizing_parameters(&shrink, &grow);
    return shrink;
  }
  void min_load_factor(float new_shrink) {
    float shrink, grow;
    rep.get_resizing_parameters(&shrink, &grow);
    rep.set_resizing_parameters(new_shrink, grow);
  }
  // Deprecated; use min_load_factor() or max_load_factor() instead.
  void set_resizing_parameters(float shrink, float grow) {
    rep.set_resizing_parameters(shrink, grow);
  }

  void resize(size_type hint) { rep.resize(hint); }
  void rehash(size_type hint) { resize(hint); }  // the tr1 name

  // Lookup routines
  iterator find(const key_type& key) { return rep.find(key); }
  const_iterator find(const key_type& key) const { return rep.find(key); }

  template <typename K>
  typename std::enable_if<sparsehash_internal::has_transparent_key_equal<hasher, K>::value, iterator>::type
  find(const K& key) { return rep.find(key); }
  template <typename K>
  typename std::enable_if<sparsehash_internal::has_transparent_key_equal<hasher, K>::value, const_iterator>::type
  find(const K& key) const { return rep.find(key); }

  data_type& operator[](const key_type& key) {  // This is our value-add!
    // If key is in the hashtable, returns find(key)->second,
    // otherwise returns insert(value_type(key, T()).first->second.
    // Note it does not create an empty T unless the find fails.
    return rep.template find_or_insert<data_type>(key).second;
  }

  data_type& operator[](key_type&& key) {
    return rep.template find_or_insert<data_type>(std::move(key)).second;
  }

  size_type count(const key_type& key) const { return rep.count(key); }

  template <typename K>
  typename std::enable_if<sparsehash_internal::has_transparent_key_equal<hasher, K>::value, size_type>::type
  count(const K& key) const { return rep.count(key); }

  std::pair<iterator, iterator> equal_range(const key_type& key) {
    return rep.equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const {
    return rep.equal_range(key);
  }

  template<typename K>
  typename std::enable_if<sparsehash_internal::has_transparent_key_equal<hasher, K>::value, std::pair<iterator, iterator>>::type
  equal_range(const K& key) {
    return rep.equal_range(key);
  }
  template<typename K>
  typename std::enable_if<sparsehash_internal::has_transparent_key_equal<hasher, K>::value, std::pair<const_iterator, const_iterator>>::type
  equal_range(const K& key) const {
    return rep.equal_range(key);
  }

  // Insertion routines
  std::pair<iterator, bool> insert(const value_type& obj) {
    return rep.insert(obj);
  }

  template <typename Pair, typename = typename std::enable_if<std::is_constructible<value_type, Pair&&>::value>::type>
  std::pair<iterator, bool> insert(Pair&& obj) {
    return rep.insert(std::forward<Pair>(obj));
  }

  // overload to allow {} syntax: .insert( { {key}, {args} } )
  std::pair<iterator, bool> insert(value_type&& obj) {
    return rep.insert(std::move(obj));
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return rep.emplace(std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace_hint(const_iterator hint, Args&&... args) {
    return rep.emplace_hint(hint, std::forward<Args>(args)...);
  }


  template <class InputIterator>
  void insert(InputIterator f, InputIterator l) {
    rep.insert(f, l);
  }
  void insert(const_iterator f, const_iterator l) { rep.insert(f, l); }
  void insert(std::initializer_list<value_type> ilist) { rep.insert(ilist.begin(), ilist.end()); }
  // Required for std::insert_iterator; the passed-in iterator is ignored.
  iterator insert(const_iterator, const value_type& obj) { return insert(obj).first; }
  iterator insert(const_iterator, value_type&& obj) { return insert(std::move(obj)).first; }
  template <class P, class = typename std::enable_if<
                                        std::is_constructible<value_type, P&&>::value &&
                                        !std::is_same<value_type, P>::value
                                      >::type>
  iterator insert(const_iterator, P&& obj) { return insert(std::forward<P>(obj)).first; }

  // Deletion and empty routines
  // THESE ARE NON-STANDARD!  I make you specify an "impossible" key
  // value to identify deleted and empty buckets.  You can change the
  // deleted key as time goes on, or get rid of it entirely to be insert-only.
   // YOU MUST CALL THIS!
  void set_empty_key(const key_type& key) { rep.set_empty_key(key); }
  key_type empty_key() const {  return rep.empty_key(); }

  void set_deleted_key(const key_type& key) { rep.set_deleted_key(key); }
  void clear_deleted_key() { rep.clear_deleted_key(); }
  key_type deleted_key() const { return rep.deleted_key(); }

  // These are standard
  size_type erase(const key_type& key) { return rep.erase(key); }
  iterator erase(const_iterator it) { return rep.erase(it); }
  iterator erase(const_iterator f, const_iterator l) { return rep.erase(f, l); }

  // Comparison
  bool operator==(const dense_hash_map& hs) const { return rep == hs.rep; }
  bool operator!=(const dense_hash_map& hs) const { return rep != hs.rep; }

  // I/O -- this is an add-on for writing hash map to disk
  //
  // For maximum flexibility, this does not assume a particular
  // file type (though it will probably be a FILE *).  We just pass
  // the fp through to rep.

  // If your keys and values are simple enough, you can pass this
  // serializer to serialize()/unserialize().  "Simple enough" means
  // value_type is a POD type that contains no pointers.  Note,
  // however, we don't try to normalize endianness.
  typedef typename ht::NopointerSerializer NopointerSerializer;

  // serializer: a class providing operator()(OUTPUT*, const value_type&)
  //    (writing value_type to OUTPUT).  You can specify a
  //    NopointerSerializer object if appropriate (see above).
  // fp: either a FILE*, OR an ostream*/subclass_of_ostream*, OR a
  //    pointer to a class providing size_t Write(const void*, size_t),
  //    which writes a buffer into a stream (which fp presumably
  //    owns) and returns the number of bytes successfully written.
  //    Note basic_ostream<not_char> is not currently supported.
  template <typename ValueSerializer, typename OUTPUT>
  bool serialize(ValueSerializer serializer, OUTPUT* fp) {
    return rep.serialize(serializer, fp);
  }

  // serializer: a functor providing operator()(INPUT*, value_type*)
  //    (reading from INPUT and into value_type).  You can specify a
  //    NopointerSerializer object if appropriate (see above).
  // fp: either a FILE*, OR an istream*/subclass_of_istream*, OR a
  //    pointer to a class providing size_t Read(void*, size_t),
  //    which reads into a buffer from a stream (which fp presumably
  //    owns) and returns the number of bytes successfully read.
  //    Note basic_istream<not_char> is not currently supported.
  // NOTE: Since value_type is std::pair<const Key, T>, ValueSerializer
  // may need to do a const cast in order to fill in the key.
  template <typename ValueSerializer, typename INPUT>
  bool unserialize(ValueSerializer serializer, INPUT* fp) {
    return rep.unserialize(serializer, fp);
  }
};

// We need a global swap as well
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline void swap(dense_hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                 dense_hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm2) {
  hm1.swap(hm2);
}

}  // namespace google
