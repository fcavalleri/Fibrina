#ifndef MATRIX__SMALL_VECTOR_HPP_
#define MATRIX__SMALL_VECTOR_HPP_

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define THROW_UNIMPLEMENTED throw std::runtime_error( AT ": unimplemented" );

namespace mtx {

namespace detail {
static constexpr size_t __align_up(size_t alignment, size_t size) {
  size_t ans = 0;
  while (ans < size)ans += alignment;
  return ans;
}
template<size_t Size>
struct uint_size;
template<>
struct uint_size<1> { typedef uint8_t type; };
template<>
struct uint_size<2> { typedef uint16_t type; };
template<>
struct uint_size<4> { typedef uint32_t type; };
template<>
struct uint_size<8> { typedef uint64_t type; };

}

template<typename T>
class small_vector {
private:

  typedef std::vector<T> vector_t;
  static_assert(sizeof(vector_t) == 3 * 8);
  struct ph_vector_t {
    void *begin, *end, *end_allocated;
  };
  static_assert(sizeof(vector_t) == sizeof(ph_vector_t));
  static_assert(alignof(vector_t) == alignof(ph_vector_t));

  // if small_capacity = k, the space will be k*sizeof(T) + align_up(2, alignof(T)), which should be <= sizeof(vector_t)
  // Hence
public:
  static constexpr size_t small_capacity = (sizeof(vector_t) - detail::__align_up(alignof(T), 2)) / sizeof(T);
  typedef T value_type;
  typedef T &reference;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef const T &const_reference;
  typedef typename vector_t::iterator iterator;
  typedef typename vector_t::const_iterator const_iterator;
private:
  struct small_t {
    //Note: it could be furtherly optimized when sizeof(T)=1 by making is_small/1, size/7.
    uint8_t is_small_byte;
    uint8_t size;
    inline bool is_small() const { return is_small_byte & 1; }
    typedef typename detail::uint_size<alignof(T)>::type utype;
    static_assert(alignof(T) == alignof(utype));
    utype buffer[small_capacity * sizeof(T) / sizeof(utype)];

    void initialize() {
      is_small_byte = 1;
      size = 0;
    }
    void initialize(std::initializer_list<T> init) {
      is_small_byte = 1;
      size = init.size();
      std::uninitialized_copy(init.begin(), init.end(), data());
    }
    void initialize(size_t n, const T &x) {
      is_small_byte = 1;
      size = n;
      std::uninitialized_fill_n(data(), n, x);
    }
    template<typename InputIt>
    void initialize(InputIt first, size_t n) {
      is_small_byte = 1;
      size = n;
      std::uninitialized_copy_n(first, n, data());
    }
    pointer data() { return reinterpret_cast<T *>(buffer); }
    const_pointer data() const { return reinterpret_cast<const T *>(buffer); }
    iterator begin() { return iterator(data()); }
    const_iterator begin() const { return const_iterator(data()); }
    iterator end() { return iterator(data() + size); }
    const_iterator end() const { return const_iterator(data() + size); }
    reference at(size_t idx) {
      if (idx >= size) {
        std::__throw_out_of_range_fmt(__N("small_vector::_M_range_check: idx "
                                          "(which is %zu) >= this->size() "
                                          "(which is %zu)"),
                                      idx, size);
      }
      return *(begin() + idx);
    }
    const_reference at(size_t idx) const {
      if (idx >= size) {
        std::__throw_out_of_range_fmt(__N("small_vector::_M_range_check: idx "
                                          "(which is %zu) >= this->size() "
                                          "(which is %zu)"),
                                      idx, size);
      }
      return *(begin() + idx);
    }
    reference operator[](size_t idx) noexcept { return *(begin() + idx); }
    const_reference operator[](size_t idx) const noexcept { return *(begin() + idx); }
    reference front() noexcept { return *begin(); }
    const_reference front() const noexcept { return *begin(); }
    reference back() noexcept { return *(begin() + size - 1); }
    const_reference back() const noexcept { return *(begin() + size - 1); }
    bool empty() const noexcept { return size == 0; }
    constexpr size_t max_size() const noexcept { return small_capacity; }
    constexpr size_t capacity() const noexcept { return small_capacity; }
    constexpr void shrink_to_fit() const noexcept {}
    constexpr void clear() noexcept {
      std::destroy_n(data(), size);
      size = 0;
    }
    void push_back(const T &x) { new(data() + size++) T(x); }
    void push_back(T &&x) { new(data() + size++) T(std::move(x)); }
    template<typename ...Args>
    void emplace_back(Args &&... args) { new(data() + size++) T(std::forward<Args>(args)...); }
    void pop_back() { std::destroy_at(data() + (--size)); }
    void swap(small_t &o) {
      if (size < o.size) {
        std::swap_ranges(data(), data() + size, o.data());
        std::uninitialized_move(o.data() + size, o.data() + o.size, data() + size);
        std::destroy(o.data() + size, o.data() + o.size);
      } else {
        std::swap_ranges(o.data(), o.data() + o.size, data());
        std::uninitialized_move(data() + o.size, data() + size, o.data() + o.size);
        std::destroy(data() + o.size, data() + size);
      }
      std::swap(size, o.size);
    }

  };
  static_assert(sizeof(vector_t) == sizeof(small_t));
  union union_t {
    small_t small;
    ph_vector_t vector;
  };
  static_assert(sizeof(union_t) == sizeof(small_t));

  union_t content;

  small_t &small() { return content.small; }
  const small_t &small() const { return content.small; }
  vector_t &vector() { return *reinterpret_cast<vector_t *>(&content.vector); }
  const vector_t &vector() const { return *reinterpret_cast<const vector_t *>(&content.vector); }
public:

  inline bool is_small() const noexcept { return content.small.is_small(); }
  small_vector() { small().initialize(); }
  small_vector(std::initializer_list<T> init) {
    if (init.size() <= small_capacity)
      small().initialize(init);
    else new(&vector()) vector_t(init);
  }
  small_vector(size_t count, const T &value = T()) {
    if (count <= small_capacity)
      small().initialize(count,
                         value);
    else new(&vector()) vector_t(count, value);
  }
  template<typename InputIt>
  void range_initialize(InputIt first, InputIt second, std::input_iterator_tag) {
    for (; first != second; ++first)emplace_back(*first);
  }
  template<typename InputIt>
  void range_initialize(InputIt first, InputIt second, std::forward_iterator_tag) {
    size_t count = std::distance(first, second);
    if (count <= small_capacity)small().initialize(first, count);
    else
      new(&vector()) vector_t(first,
                              second);
  }
  template<typename InputIt>
  small_vector(InputIt first, InputIt last) { range_initialize(first, last, std::__iterator_category(first)); }

  iterator begin() { return is_small() ? small().begin() : vector().begin(); }
  const_iterator begin() const { return is_small() ? small().begin() : vector().cbegin(); }
  const_iterator cbegin() const { return begin(); }

  iterator end() { return is_small() ? small().end() : vector().end(); }
  const_iterator end() const { return is_small() ? small().end() : vector().cend(); }
  const_iterator cend() const { return end(); }

  size_t capacity() const { return is_small() ? small_capacity : vector().capacity(); }

  size_t size() const { return is_small() ? small().size : vector().size(); }

  void reserve(size_t new_cap) {
    if (is_small()) {
      if (new_cap > small_capacity) {
        //TODO: grow to vector
        THROW_UNIMPLEMENTED
      }
    } else vector().reserve(new_cap);
  }

  void resize(size_t count) {
    if (is_small()) {
      if (count < content.small.size) {
        //TODO: destroy some, update size
        THROW_UNIMPLEMENTED
      } else if (count > content.small.size) {
        //increase size
        if (count > small_capacity) {
          //TODO: grow to vector
          THROW_UNIMPLEMENTED
        } else {
          //allocate new element
        }

      }
    } else {

    }
  }

  iterator erase(const_iterator it) {
    if (is_small()) {
      std::move(const_cast<T *>(&*it) + 1, reinterpret_cast<T *>(small().buffer) + small().size, const_cast<T *>(&*it));
      std::destroy_at(reinterpret_cast<T *>(small().buffer) + (--small().size));
      return iterator(const_cast<T *>(&*it));
    } else {
      return vector().erase(it);
    }
  }

  void unordered_erase(const_iterator it) {
    //TODO: just copy the last in its place instead
    erase(it);
  }

  void push_back(T &&x) {
    if (is_small()) {
      if (small().size == small_capacity) {
        //grow to vector
        vector_t v;
        v.reserve(small_capacity * 2);
        std::uninitialized_move_n(reinterpret_cast<T *>(small().buffer), small_capacity, v.data());
        std::destroy_n(reinterpret_cast<T *>(small().buffer), small_capacity);

        new((void *) &vector())  vector_t(std::move(v));
        content.vector.end = reinterpret_cast<void *>(reinterpret_cast<T *>(content.vector.begin) + small_capacity);
        vector().push_back(std::move(x));
      } else {
        new(reinterpret_cast<T *>(small().buffer) + (small().size++)) T(std::move(x));
      }
    } else {
      vector().push_back(std::move(x));
    }
  }

  void push_back(const T &x) {
    if (is_small()) {
      if (small().size == small_capacity) {
        //grow to vector
        vector_t v;
        v.reserve(small_capacity * 2);
        std::uninitialized_move_n(reinterpret_cast<T *>(small().buffer), small_capacity, v.data());
        std::destroy_n(reinterpret_cast<T *>(small().buffer), small_capacity);

        new((void *) &vector())  vector_t(std::move(v));
        content.vector.end = reinterpret_cast<void *>(reinterpret_cast<T *>(content.vector.begin) + small_capacity);
        vector().push_back(x);
      } else {
        new(reinterpret_cast<T *>(small().buffer) + (small().size++)) T(x);
      }
    } else {
      vector().push_back(x);
    }
  }

  template<typename ...Args>
  void emplace_back(Args &&... args) {
    if (is_small()) {
      if (small().size == small_capacity) {
        //grow to vector
        vector_t v;
        v.reserve(small_capacity * 2);
        std::uninitialized_move_n(reinterpret_cast<T *>(small().buffer), small_capacity, v.data());
        new((void *) &vector())  vector_t(std::move(v));
        content.vector.end = reinterpret_cast<void *>(reinterpret_cast<T *>(content.vector.begin) + small_capacity);
        std::destroy_n(reinterpret_cast<T *>(small().buffer), small_capacity);

        vector().push_back(std::forward<Args>(args)...);
      } else {
        new(reinterpret_cast<T *>(small().buffer) + (small().size++)) T(std::forward<Args>(args)...);
      }
    } else {
      vector().push_back(std::forward<Args>(args)...);
    }
  }

  reference at(size_t idx) { return is_small() ? small().at(idx) : vector().at(idx); }
  const_reference at(size_t idx) const { return is_small() ? small().at(idx) : vector().at(idx); }
  reference operator[](size_t idx) { return is_small() ? small()[idx] : vector()[idx]; }
  const_reference operator[](size_t idx) const { return is_small() ? small()[idx] : vector()[idx]; }
  reference front() { return is_small() ? small().front() : vector().front(); }
  const_reference front() const { return is_small() ? small().front() : vector().front(); }
  reference back() { return is_small() ? small().back() : vector().back(); }
  const_reference back() const { return is_small() ? small().back() : vector().back(); }

  ~small_vector() {
    if (is_small()) {
      std::destroy_n(reinterpret_cast<T *>(small().buffer), small().size);
    } else {
      std::destroy_at(&vector());
    }
  }

};

}

#endif //MATRIX__SMALL_VECTOR_HPP_
