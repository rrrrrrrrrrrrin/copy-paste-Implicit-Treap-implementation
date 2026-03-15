#ifndef VECTOR_H
#include <cstdint>  // For uint64_t

const int size_array = 100;

template <typename T>
struct Pair {
  T array[2] = {0};

  T& operator[](uint64_t x) {
    if (x > 1) {
      throw "vector.h Pair: index out of range";
    }
    return array[x];
  }

  const T& operator[](uint64_t x) const {
    if (x > 1) {
      throw "vector.h Pair: index out of range";
    }
    return array[x];
  }
};

template <typename T>
class Vector {
 private:
  T* Data;
  uint64_t capacity = 0;
  uint64_t size = 0;

 public:
  // Constructors
  Vector() {
    size = 0;
    capacity = 10;
    Data = new T[capacity];
  }

  Vector(uint64_t size) {
    this->size = 0;
    this->capacity = size;
    Data = new T[capacity];
  }

  // Copy constructor
  Vector(const Vector& other) {
    size = other.size;
    capacity = other.capacity;
    Data = new T[capacity];
    for (int i = 0; i < size; ++i) {
      Data[i] = other.Data[i];
    }
  }

  // Assignment operator
  Vector& operator=(const Vector& other) {
    if (this == &other) {
      return *this;
    }
    size = other.size;
    capacity = other.capacity;
    for (int i = 0; i < size; ++i) {
      Data[i] = other.Data[i];
    }
    return *this;
  }

  ~Vector() { delete[] Data; };

  uint64_t get_size() const { return size; }

  bool empty() { return size == 0; }

  void push_back(const T& value) {
    if (capacity == size) {
      capacity = 2 * capacity;
      T* newData = new T[capacity];

      for (uint64_t i = 0; i < size; ++i) {
        newData[i] = Data[i];
      }

      delete[] Data;
      Data = newData;
    }

    Data[size] = value;
    size++;
  }

  void reverse() {
    T* newData = new T[capacity];
    for (int64_t i = static_cast<int64_t>(size) - 1; i >= 0; i--) {
      newData[size - i - 1] = Data[i];
    }
    delete[] Data;
    Data = newData;
  }

  void fill(const T& elem) {
    for (uint64_t i = 0; i < capacity; i++) {
      Data[i] = elem;
    }
    size = capacity;
  }

  T& top() {
    if (size == 0) {
      throw "vector.h Vector, top(): vector is empty";
    }
    return Data[size - 1];
  }

  void pop() {
    if (size == 0) {
      throw "vector.h Vector, pop(): vector is empty";
    }
    --size;
  }

  T& operator[](uint64_t x) {
    if (x > size - 1) {
      throw "vector.h Vector: index out of range";
    }
    return Data[x];
  }

  const T& operator[](uint64_t x) const {
    if (x > size - 1) {
      throw "vector.h Vector: index out of range";
    }
    return Data[x];
  }
};

#endif