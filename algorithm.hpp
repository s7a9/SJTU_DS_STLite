#ifndef STLITE_ALGORITHM_HPP
#define STLITE_ALGORITHM_HPP


template <typename T> 
struct remove_reference
{ typedef T type; };
 
template <class T> 
struct remove_reference<T&>
{ typedef T type; };
 
template <class T> 
struct remove_reference<T&&> 
{ typedef T type; };

template <class T>
typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}

template <typename T>
void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

#endif // STLITE_ALGORITHM_HPP