#pragma once
#include <cctype>

template < typename _type_ >
struct CharTraitsBase {
	_type_ IsDigit( const _type_ c ) const { return false; }
	_type_ IsAlphabetic( const _type_ c ) const { return false; }
	_type_ ToLower( const _type_ c ) const { return false; }
};

template <>
struct CharTraitsBase< char > {
	static bool IsDigit( const char c ) { return isdigit( c ); }
	static bool IsUpper( const char c ) { return isupper( c ); }
	static bool IsLower( const char c ) { return islower( c ); }
	static bool IsAlphabetic( const char c ) { return isalpha( c ); }
	static bool IsAlphanumeric( const char c ) { return isalnum( c ); }
	static char ToLower( const char c ) { return static_cast< char >( tolower( c ) ); }
};


template <>
struct CharTraitsBase< wchar_t > {
	static bool IsDigit( const wchar_t c ) { return iswdigit( c ); }
	static bool IsUpper( const wchar_t c ) { return iswupper( c ); }
	static bool IsLower( const wchar_t c ) { return iswlower( c ); }
	static bool IsAlphabetic( const wchar_t c ) { return iswalpha( c ); }
	static bool IsAlphanumeric( const wchar_t c ) { return iswalnum( c ); }
	static wchar_t ToLower( const wchar_t c ) { return towlower( c ); }
};

template < typename _type_ >
struct CharTraits : public CharTraitsBase< _type_ > {};

template <>
struct CharTraits< char > : public CharTraitsBase< char > {};
template <>
struct CharTraits< unsigned char > : public CharTraitsBase< char > {};
template <>
struct CharTraits< wchar_t > : public CharTraitsBase< char > {};