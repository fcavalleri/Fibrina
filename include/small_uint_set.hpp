#ifndef FIBRINA_INCLUDE_SMALL_UINT_SET_H_
#define FIBRINA_INCLUDE_SMALL_UINT_SET_H_

namespace mtx {

template<typename T, size_t MaxSize>
class small_uint_set {
public:
  small_uint_set() : size_(0) {}
  void insert_safe(T x) {
    if (size_ == MaxSize)throw std::runtime_error("Small Set Size exceeded!");
    data_[size_++] = x;
  }
  void insert(T x) noexcept {
    data_[size_++] = x;
  }
  void erase(T x) noexcept {
    *std::find(data_.begin(), data_.end(), x) = data_[--size_];
  }
  void erase_safe(T x) noexcept {
    auto it = std::find(data_.begin(), data_.begin() + size_, x);
    if (it == data_.begin() + size_)return;
    *it = data_[--size_];
  }
  uint8_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_==0;}
  auto begin() const noexcept { return data_.cbegin(); }
  auto end() const noexcept { return data_.cbegin() + size_; }
private:
  uint8_t size_;
  std::array<T, MaxSize> data_;
};

}

#endif //FIBRINA_INCLUDE_SMALL_UINT_SET_H_
