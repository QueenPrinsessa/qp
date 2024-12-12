#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/string/qp_string.h"
#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
	WideString UTF8ToWide( const char * string, const int length ) {
		int wideStrLength = ::MultiByteToWideChar( CP_UTF8, 0, string, length, NULL, 0 );

		if ( wideStrLength == 0 ) {
			return WideString {};
		}

		WideString convertedString( wideStrLength, L'\0' );
		::MultiByteToWideChar( CP_UTF8, 0, string, length, convertedString.Data(), wideStrLength );
		return convertedString;
	}

	WideString UTF8ToWide( const String & string ) {
		return UTF8ToWide( string.c_str(), string.Length() );
	}

	inline WideString UTF8ToWide( const U8String & string ) {
		return UTF8ToWide( reinterpret_cast< char * >( string.Data() ), string.Length() );
	}

	U8String WideToUTF8( const wchar_t * string, const int length ) {
		int numBytes = ::WideCharToMultiByte( CP_UTF8, 0, string, length, NULL, 0, NULL, NULL );

		if ( numBytes == 0 ) {
			return U8String {};
		}

		U8String convertedString( numBytes + 1, '\0' );
		::WideCharToMultiByte( CP_UTF8, 0, string, length, reinterpret_cast< char * >( convertedString.Data() ), numBytes, NULL, NULL );
		return convertedString;
	}

	inline U8String WideToUTF8( const WideString & string ) {
		return WideToUTF8( string.c_str(), string.Length() );
	}

	WideString UTF8ToWide( const char c ) {
		return UTF8ToWide( &c, 1 );
	}

	U8String WideToUTF8( const wchar_t c ) {
		return WideToUTF8( &c, 1 );
	}
}
#endif
