#ifndef STLITE_UTILITIES_HPP
#define STLITE_UTILITIES_HPP

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

    template <typename T1, typename T2>
    inline T1 val_max(T1 a, T2 b) {
        if (a < b) return b;
        else return a;
    }

    template <typename T1, typename T2>
    inline T1 val_min(T1 a, T2 b) {
        if (a < b) return a;
        else return b;
    }
}

#endif // STLITE_UTILITIES_HPP