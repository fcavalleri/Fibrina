#ifndef FIBRINA_INCLUDE_MATRIX_H_
#define FIBRINA_INCLUDE_MATRIX_H_

#include <array>
#include <iostream>

namespace mtx {

template<typename T, size_t N, size_t ...Ns>
class matrix;

template<typename T, size_t N>
class matrix<T, N> : public std::array<T, N> {
private:
  template<typename U, size_t M, size_t ...Ms> friend
  class matrix;
  typedef std::array<T, N> base_t;
  static constexpr size_t _items_size = N;
  std::ostream &print(std::ostream &os) const;
public:
  //constructors
  constexpr explicit matrix() : base_t() {}
  constexpr explicit matrix(const matrix &) = default;
  constexpr explicit matrix(matrix &&) = default;
  constexpr explicit matrix(const base_t &o) : base_t(o) {}
  constexpr explicit matrix(const base_t &&o) : base_t(std::move(o)) {}
  //assigners
  constexpr matrix &operator=(const matrix &) noexcept = default;
  constexpr matrix &operator=(matrix &&) noexcept = default;
  constexpr matrix &operator=(const base_t &o) noexcept;;
  constexpr matrix &operator=(base_t &&o) noexcept;;

  //size
  [[nodiscard]] constexpr matrix<size_t, 1> shape() const noexcept;
  [[nodiscard]] constexpr size_t order() const noexcept { return 1; }

  //projection
  template<size_t ...Ws>
  constexpr matrix<T, Ws...> &reshape() noexcept;
  template<size_t ...Ws>
  constexpr const matrix<T, Ws...> &reshape() const noexcept;
  matrix<T, _items_size> &flat() noexcept;
  const matrix<T, _items_size> &flat() const noexcept;
  template<size_t W>
  constexpr matrix<T, W> &slice(size_t start);
  template<size_t begin, size_t end>
  constexpr matrix<T, end - begin, N> &slice() noexcept;
  friend std::ostream &operator<<(std::ostream &os, const matrix<T, N> &m) { return m.print(os); }
};

template<typename T, size_t N1, size_t N2, size_t ...Ns>
class matrix<T, N1, N2, Ns...> : public std::array<matrix<T, N2, Ns...>, N1> {
  template<typename U, size_t M, size_t ...Ms> friend
  class matrix;
public:
  typedef matrix<T, N2, Ns...> row_t;

private:
  typedef std::array<row_t, N1> base_t;
  static constexpr size_t _items_size = ((N1 * N2) * ... * Ns);
  static constexpr size_t _order = 2 + (sizeof...(Ns));

public:

  //constructors
  constexpr explicit matrix() : base_t() {}
  constexpr explicit matrix(const matrix &) = default;
  constexpr explicit matrix(matrix &&) = default;
  //assigners
  constexpr matrix &operator=(const matrix &) noexcept = default;
  constexpr matrix &operator=(matrix &&) noexcept = default;

  //size
  constexpr matrix<size_t, _order> shape() const noexcept;

  template<size_t W>
  constexpr matrix<T, W, N2, Ns...> &slice(size_t start);
  template<size_t begin, size_t end>
  constexpr matrix<T, end - begin, N2, Ns...> &slice() noexcept;
  template<size_t ...Ws>
  constexpr matrix<T, Ws...> &reshape() noexcept;
  matrix<T, _items_size> &flat() noexcept { return reshape<_items_size>(); }
  const matrix<T, _items_size> &flat() const noexcept { return reshape<_items_size>(); }
  constexpr size_t order() const noexcept { return 2 + sizeof...(Ns); }

  friend std::ostream &operator<<(std::ostream &os, const matrix<T, N1, N2, Ns...> &m) { return m.print(os); }

private:

  std::ostream &print(std::ostream &os, size_t indent = 0) const;
};

// matrix<T,N> implementation
template<typename T, size_t N>
template<size_t... Ws>
constexpr const matrix<T, Ws...> &matrix<T, N>::reshape() const noexcept {
  static_assert(matrix<T, N>::_items_size == matrix<T, Ws...>::_items_size);
  static_assert(sizeof(matrix<T, N>) == sizeof(matrix<T, Ws...>));
  return *reinterpret_cast<matrix<T, Ws...> *>(this);
}
template<typename T, size_t N>
template<size_t... Ws>
constexpr matrix<T, Ws...> &matrix<T, N>::reshape() noexcept {
  static_assert(matrix<T, N>::_items_size == matrix<T, Ws...>::_items_size);
  static_assert(sizeof(matrix<T, N>) == sizeof(matrix<T, Ws...>));
  return *reinterpret_cast<matrix<T, Ws...> *>(this);
}
template<typename T, size_t N>
matrix<T, matrix<T, N>::_items_size> &matrix<T, N>::flat() noexcept { return reshape<_items_size>(); }
template<typename T, size_t N>
const matrix<T, matrix<T, N>::_items_size> &matrix<T, N>::flat() const noexcept { return reshape<_items_size>(); }
template<typename T, size_t N>
constexpr matrix<size_t, 1> matrix<T, N>::shape() const noexcept {
  return matrix<size_t, 1>({N});
}

template<typename T, size_t N>
std::ostream &matrix<T, N>::print(std::ostream &os) const {
  os << "[";
  if constexpr (N > 0) {
    os << base_t::at(0);
    for (size_t i = 1; i < N; ++i)os << ", " << base_t::at(i);
  }
  return os << "]";
}
template<typename T, size_t N>
constexpr matrix<T, N> &matrix<T, N>::operator=(const std::array<T, N> &o) noexcept {
  base_t::operator=(o);
  return *this;
}
template<typename T, size_t N>
constexpr matrix<T, N> &matrix<T, N>::operator=(std::array<T, N> &&o) noexcept {
  base_t::operator=(std::move(o));
  return *this;
}
template<typename T, size_t N>
template<size_t begin, size_t end>
constexpr matrix<T, end - begin, N> &matrix<T, N>::slice() noexcept {
  static_assert(begin <= end);
  static_assert(end <= N);
  return *reinterpret_cast<matrix<T, end - begin, N> *>((reinterpret_cast<T *>(this) + begin));
}
template<typename T, size_t N>
template<size_t W>
constexpr matrix<T, W> &matrix<T, N>::slice(size_t start) {
  static_assert(W <= N);
  if (start + W > N)
    std::__throw_out_of_range_fmt("array::at: start (which is %zu) + W (which is %zu)  > N (which is %zu)",
                                  start,
                                  W,
                                  N);
  return *reinterpret_cast<matrix<T, W> *>((reinterpret_cast<T *>(this) + start));
}

// matrix<T,N,Ns...> implementaion
template<typename T, size_t N1, size_t N2, size_t... Ns>
constexpr matrix<size_t, matrix<T, N1, N2, Ns...>::_order> matrix<T, N1, N2, Ns...>::shape() const noexcept {
  return matrix<size_t, _order>({N1, N2, Ns...});
}

template<typename T, size_t N1, size_t N2, size_t... Ns>
template<size_t begin, size_t end>
constexpr matrix<T, end - begin, N2, Ns...> &matrix<T, N1, N2, Ns...>::slice() noexcept {
  static_assert(begin <= end);
  static_assert(end <= N1);
  return *reinterpret_cast<matrix<T, end - begin, N2, Ns...> *>((reinterpret_cast<row_t *>(this) + begin));
}

template<typename T, size_t N1, size_t N2, size_t... Ns>
template<size_t W>
constexpr matrix<T, W, N2, Ns...> &matrix<T, N1, N2, Ns...>::slice(size_t start) {
  static_assert(W <= N1);
  if (start + W > N1)
    std::__throw_out_of_range_fmt("array::at: start (which is %zu) + W (which is %zu)  > N (which is %zu)",
                                  start,
                                  W,
                                  N1);
  return *reinterpret_cast<matrix<T, W, N2, Ns...> *>((reinterpret_cast<row_t *>(this) + start));
}

template<typename T, size_t N1, size_t N2, size_t... Ns>
template<size_t... Ws>
constexpr matrix<T, Ws...> &matrix<T, N1, N2, Ns...>::reshape() noexcept {
  static_assert(matrix<T, N1, N2, Ns...>::_items_size == matrix<T, Ws...>::_items_size);
  static_assert(sizeof(matrix<T, N1, N2, Ns...>) == sizeof(matrix<T, Ws...>));
  return *reinterpret_cast<matrix<T, Ws...> *>(this);
}

template<typename T, size_t N1, size_t N2, size_t... Ns>
std::ostream &matrix<T, N1, N2, Ns...>::print(std::ostream &os, size_t indent) const {
  if constexpr (N1 == 0)return os << "[]";

  if constexpr ((sizeof...(Ns))) {
    if constexpr (N1 == 1) {
      os << "[";
      base_t::at(0).print(os, indent + 1);
      return os << "]";
    }
    os << "[";
    base_t::at(0).print(os, indent + 1);
    os << ",";
    for (size_t _rc = (sizeof...(Ns)) + 1; _rc--;)os << "\n";

    for (size_t i = 1; i + 1 < N1; ++i) {
      for (size_t _rc = indent + 1; _rc--;)os << " ";
      base_t::at(N1 - 1).print(os, indent + 1);
      os << ",";
      for (size_t _rc = (sizeof...(Ns)) + 1; _rc--;)os << "\n";
    }

    for (size_t _rc = indent + 1; _rc--;)os << " ";
    base_t::at(N1 - 1).print(os, indent + 1);
    return os << "]";

  } else {
    static_assert((sizeof...(Ns)) == 0);

    if constexpr (N1 == 1) {
      os << "[";
      base_t::at(0).print(os);
      return os << "]";
    }

    os << "[";
    base_t::at(0).print(os);
    os << ",\n";

    for (size_t i = 1; i + 1 < N1; ++i) {
      for (size_t _rc = indent + 1; _rc--;)os << " ";
      base_t::at(i).print(os);
      os << ",\n";
    }
    for (size_t _rc = indent + 1; _rc--;)os << " ";
    base_t::at(N1 - 1).print(os);
    return os << "]";
  }

}

}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  os << "[";
  if (v.empty())return os << "]";
  os << v.front();
  for (size_t i = 1; i < v.size(); ++i)os << ", " << v.at(i);
  return os << "]";
}

template<typename InputIt>
std::ostream &print_range(std::ostream &os, InputIt first, InputIt last) {
  os << "[";
  bool comma = false;
  while (first != last) {
    if (comma)os << ", ";
    comma = true;
    os << *(first++);
  }
  return os << "]";
}

#endif //FIBRINA_INCLUDE_MATRIX_H_
