#ifndef STLITE_ALGORITHM_HPP
#define STLITE_ALGORITHM_HPP

namespace s7a9 {
    template <typename T> struct RemoveReference {
        typedef T type;
    };
    template <typename T> struct RemoveReference<T&> {
        typedef T type;
    };
    template <typename T> struct RemoveReference<T&&> {
        typedef T type;
    };

    template <typename T>
    typename RemoveReference<T>::type&& Move(T&& t) {
        return static_cast<typename RemoveReference<T>::type&&>(t);
    }

    template <typename T>
    inline void swap(T& a, T& b) {
        T tmp(a);
        a = b;
        b = tmp;
    }
}

#endif // STLITE_ALGORITHM_HPP