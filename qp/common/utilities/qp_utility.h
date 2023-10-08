#pragma once

template < typename T >
struct removeReference_t {
	using type = T;
	using constType = const T;
};

template < typename T >
struct removeReference_t< T & >
{
	using type = T;
	using constType = const T &;
};

template < typename T >
struct removeReference_t< T && >
{
	using type = T;
	using constType = const T &&;
};

template < typename T >
using removeReferenceType = typename removeReference_t< T >::type;

template < typename T >
using removeReferenceConstType = typename removeReference_t< T >::type;

template < typename T >
[[ nodiscard ]] T & qpForward( removeReferenceType< T > & arg ) noexcept {
	return static_cast< T & >( arg );
}

template < typename T >
[[ nodiscard ]] T && qpForward( removeReferenceType< T > && arg ) noexcept {
	return static_cast< T && >( arg );
}

template <class T>
[[ nodiscard ]] removeReferenceType< T > && qpMove( T && arg ) noexcept { 
	return static_cast< removeReferenceType< T > && >( arg );
}