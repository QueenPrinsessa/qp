#pragma once
#include "qp_macros.h"

template < bool _condition_, typename _type_ >
struct EnableIf {
};

template < typename _type_ >
struct EnableIf< true, _type_ > {
	using type = _type_;
};

template < typename _type_ >
struct RemoveReference {
	using type = _type_;
	using constType = const _type_;
};

template < typename _type_ >
struct RemoveReference< _type_ & > {
	using type = _type_;
	using constType = const _type_ &;
};

template < typename _type_ >
struct RemoveReference< _type_ && > {
	using type = _type_;
	using constType = const _type_ &&;
};

template < typename _type_ >
using removeReference_t = typename RemoveReference< _type_ >::type;

template < typename _type_ >
using removeReferenceConst_t = typename RemoveReference< _type_ >::constType;

template < typename _type_ >
struct RemoveConst {
	using type = _type_;
};

template < typename _type_ >
struct RemoveConst< const _type_ > {
	using type = _type_;
};

template < typename _type_ >
struct RemoveConstVolatile {
	using type = _type_;
};

template < typename _type_ >
struct RemoveConstVolatile< const _type_ > {
	using type = _type_;
};

template < typename _type_ >
struct RemoveConstVolatile< volatile _type_ > {
	using type = _type_;
};

template < typename _type_ >
struct RemoveConstVolatile< const volatile _type_ > {
	using type = _type_;
};

template < typename _type_ >
using removeConstVolatile_t = typename RemoveConstVolatile< _type_ >::type;

template < typename, typename >
QP_INLINE constexpr bool IsSame = false;//std::is_same_v< _type1_, _type2_ >;

template < typename _type_ >
QP_INLINE constexpr bool IsSame< _type_, _type_ > = true;

template < typename _type_, typename ... _types_ >
QP_INLINE constexpr bool IsAnyOf = ( IsSame< _type_, _types_ > || ... );

template < typename _type_ >
QP_INLINE constexpr bool IsIntegral = IsAnyOf< removeConstVolatile_t< _type_ >, bool, char, signed char, unsigned char, wchar_t,
	char8_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long >;

template < typename _type_ >
QP_INLINE constexpr bool IsFloatingPoint = IsAnyOf< removeConstVolatile_t< _type_ >, float, double, long double >;

template < typename _base_, typename _derived_ >
QP_INLINE constexpr bool IsBaseOf = __is_base_of( _base_, _derived_ );

template < typename _type_ >
QP_INLINE constexpr bool IsTrivialToCopy = __is_trivially_copyable( _type_ );
