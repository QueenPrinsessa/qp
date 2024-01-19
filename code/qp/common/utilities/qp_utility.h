#pragma once
#include "qp/common/core/qp_macros.h"
#include <type_traits>

template < typename T >
struct removeReference_t {
	using type = T;
	using constType = const T;
};

template < typename T >
struct removeReference_t< T & > {
	using type = T;
	using constType = const T &;
};

template < typename T >
struct removeReference_t< T && > {
	using type = T;
	using constType = const T &&;
};

template < typename T >
using removeReferenceType_t = typename removeReference_t< T >::type;

template < typename T >
using removeReferenceConstType_t = typename removeReference_t< T >::type;

template < typename T >
QP_NO_DISCARD constexpr T & qpForward( removeReferenceType_t< T > & arg ) noexcept {
	return static_cast< T & >( arg );
}

template < typename T >
QP_NO_DISCARD constexpr T && qpForward( removeReferenceType_t< T > && arg ) noexcept {
	return static_cast< T && >( arg );
}

template < typename T >
QP_NO_DISCARD constexpr removeReferenceType_t< T > && qpMove( T && arg ) noexcept {
	return static_cast< removeReferenceType_t< T > && >( arg );
}

template < typename T >
QP_INLINE constexpr bool qpIsIntegral = std::is_integral_v< T >;

template < typename T >
QP_INLINE constexpr bool qpIsFloatingPoint = std::is_floating_point_v< T >;

template < typename B, typename D >
QP_INLINE constexpr bool qpIsBaseOf = std::is_base_of_v< B, D >;

template < typename T >
QP_INLINE constexpr bool qpIsTrivialToCopy = std::is_trivially_copyable_v< T >;

template < typename T, typename I >
QP_INLINE constexpr bool qpIsSame = std::is_same_v< T, I >;

template < typename T, typename ... ARGS >
constexpr bool qpIsPartOfParameterPack_Internal() {
	bool found = false;
	( [ & ] () {
		if constexpr ( qpIsSame< T, ARGS > ) {
			found = true;
			return;
		}
	}( ), ... );
	return found;
}

template < typename T, typename ... ARGS >
constexpr bool qpIsPartOfParameterPack = qpIsPartOfParameterPack_Internal< T, ARGS... >();