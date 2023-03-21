#ifndef STLITE_UTILITIES_HPP
#define STLITE_UTILITIES_HPP

namespace sjtu {
    template<class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;
        constexpr pair() : first(), second() {}
        pair(const pair& other) = default;
        pair(pair&& other) = default;
        pair(const T1& x, const T2& y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(U1&& x, U2&& y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(const pair<U1, U2>& other) : first(other.first), second(other.second) {}
        template<class U1, class U2>
        pair(pair<U1, U2>&& other) : first(other.first), second(other.second) {}
    };

}

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

    template <class T1, class T2>
    using pair = sjtu::pair<T1, T2>;
}

#endif // STLITE_UTILITIES_HPP