#include <iostream>
#include <memory>

using namespace std;

template <typename T>
class SharedPtr {
 public:
  T *ptr;
  int *refCount;
  SharedPtr(T *ptr) : ptr(ptr) { refCount = new int(1); }

  void decrementCount() {
    if (refCount == nullptr) return;

    (*refCount)--;
    if (*refCount == 0) {
      delete ptr;
      refCount = nullptr;
    }
  }

  ~SharedPtr() { decrementCount(); }

  SharedPtr(const SharedPtr &o) : ptr(o.ptr), refCount(o.refCount) {
    (*refCount)++;
  }

  SharedPtr &operator=(const SharedPtr &o) {
    if (this != &o) {
      decrementCount();
      ptr = o.ptr;
      refCount = o.refCount;
      (*refCount)++;
    }
    return *this;
  }

  SharedPtr(SharedPtr &&o) : ptr(o.ptr), refCount(o.refCount) {
    o.ptr = nullptr;
    o.refCount = nullptr;
  }

  SharedPtr &operator=(SharedPtr &&o) {
    if (this != &o) {
      ptr = o.ptr;
      refCount = o.refCount;

      o.ptr = nullptr;
      o.refCount = nullptr;
    }
    return *this;
  }
};

int main() {
  {
    SharedPtr<int> sp{new int{5}};  // calls destructor
  }

  SharedPtr<int> sp1{new int{3}};
  SharedPtr<int> sp2{sp1};
  cout << *sp1.refCount << " " << *sp2.refCount << "\n";  // 2 2

  SharedPtr<int> sp3{sp2};  // copy constructor
  cout << *sp1.refCount << " " << *sp2.refCount << " " << *sp3.refCount
       << "\n";                    // 3 3 3
  SharedPtr<int> sp4{new int{2}};  // refCount 1

  sp3 = sp4;  // copy assign

  cout << *sp1.refCount << " " << *sp2.refCount << " " << *sp3.refCount << " "
       << *sp4.refCount << "\n";
  // 2 2 2 2

  SharedPtr<int> sp5{new int{1}};
  sp5 = sp5;  // copy assign

  SharedPtr<int> sp6 = move(sp5);  // move ctor, xvalue
  auto f = []() -> SharedPtr<int> { return SharedPtr<int>{new int{5}}; };

  SharedPtr<int> sp7 = f();  // move ctor, prvalue
  sp7 = move(sp6);           // move assign

  return 0;
}
