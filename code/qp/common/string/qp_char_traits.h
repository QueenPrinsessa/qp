#pragma once
#include <cctype>
#include <cwctype>
#include "qp/common/utilities/qp_algorithms.h"

namespace qp {
	enum class stringEncoding_t {
		DEFAULT,
		ASCII,
		UTF8,
		UTF16,
		UTF32
	};

	template < typename _type_ >
	struct CharTraitsBase {};

	template <>
	struct CharTraitsBase< char > {
		static bool IsDigit( const char c ) { return isdigit( c ); }
		static bool IsUpper( const char c ) { return isupper( c ); }
		static bool IsLower( const char c ) { return islower( c ); }
		static bool IsAlphabetic( const char c ) { return isalpha( c ); }
		static bool IsAlphanumeric( const char c ) { return isalnum( c ); }
		static char ToLower( const char c ) { return static_cast< char >( tolower( c ) ); }
		enum : char { NIL_CHAR = '\0' };
		static inline const stringEncoding_t DEFAULT_STRING_ENCODING = stringEncoding_t::ASCII;
	};

	template <>
	struct CharTraitsBase< unsigned char > {
		static bool IsDigit( const unsigned char c ) { return isdigit( c ); }
		static bool IsUpper( const unsigned char c ) { return isupper( c ); }
		static bool IsLower( const unsigned char c ) { return islower( c ); }
		static bool IsAlphabetic( const unsigned char c ) { return isalpha( c ); }
		static bool IsAlphanumeric( const unsigned char c ) { return isalnum( c ); }
		static unsigned char ToLower( const unsigned char c ) { return static_cast< unsigned char >( tolower( c ) ); }
		enum : unsigned char { NIL_CHAR = 0 };
		static inline const stringEncoding_t DEFAULT_STRING_ENCODING = stringEncoding_t::ASCII;
	};

	template <>
	struct CharTraitsBase< char8_t > {
		//static bool IsDigit( const char8_t c ) { return isdigit( c ); }
		//static bool IsUpper( const char8_t c ) { return isupper( c ); }
		//static bool IsLower( const char8_t c ) { return islower( c ); }
		//static bool IsAlphabetic( const char8_t c ) { return isalpha( c ); }
		//static bool IsAlphanumeric( const char8_t c ) { return isalnum( c ); }
		//static char ToLower( const char8_t c ) { return static_cast< char >( tolower( c ) ); }
		enum : char { NIL_CHAR = u8'\0' };
		static inline const stringEncoding_t DEFAULT_STRING_ENCODING = stringEncoding_t::UTF8;
	};

	template <>
	struct CharTraitsBase< wchar_t > {
		static bool IsDigit( const wchar_t c ) { return iswdigit( VerifyStaticCast< wint_t >( c ) ); }
		static bool IsUpper( const wchar_t c ) { return iswupper( VerifyStaticCast< wint_t >( c ) ); }
		static bool IsLower( const wchar_t c ) { return iswlower( VerifyStaticCast< wint_t >( c ) ); }
		static bool IsAlphabetic( const wchar_t c ) { return iswalpha( VerifyStaticCast< wint_t >( c ) ); }
		static bool IsAlphanumeric( const wchar_t c ) { return iswalnum( VerifyStaticCast< wint_t >( c ) ); }
		static wchar_t ToLower( const wchar_t c ) { return VerifyStaticCast< wchar_t >( towlower( VerifyStaticCast< wint_t >( c ) ) ); }
		enum : wchar_t { NIL_CHAR = L'\0' };
#if defined( QP_PLATFORM_WINDOWS )
		static inline const stringEncoding_t DEFAULT_STRING_ENCODING = stringEncoding_t::UTF16;
#elif defined( QP_PLATFORM_LINUX )
		static inline const stringEncoding_t DEFAULT_STRING_ENCODING = stringEncoding_t::UTF32;
#else
#error "Default string encoding for platform hasn't been setup"
#endif
	};

	template < typename _type_ >
	struct CharTraits : public CharTraitsBase< _type_ > {};
}