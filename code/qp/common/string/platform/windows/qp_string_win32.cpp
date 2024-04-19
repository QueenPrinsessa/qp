#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/string/qp_string.h"
#include "qp/common/platform/windows/qp_windows.h"

qpWideString qpUTF8ToWide( const char * string, const int length ) {
	int wideStrLength = ::MultiByteToWideChar( CP_UTF8, 0, string, length, NULL, 0 );

	if ( wideStrLength == 0 ) {
		return qpWideString {};
	}

	qpWideString convertedString( wideStrLength + 1, L'\0' );
	convertedString.Resize( wideStrLength );
	::MultiByteToWideChar( CP_UTF8, 0, string, length, convertedString.Data(), wideStrLength );
	return convertedString;
}

qpWideString qpUTF8ToWide( const qpString & string ) {
	return qpUTF8ToWide( string.c_str(), string.Length() );
}

inline qpWideString qpUTF8ToWide( const qpU8String & string ) {
	return qpUTF8ToWide( reinterpret_cast< char * >( string.Data() ), string.Length() );
}

qpU8String qpWideToUTF8( const wchar_t * string, const int length ) {
	int numBytes = ::WideCharToMultiByte( CP_UTF8, 0, string, length, NULL, 0, NULL, NULL );

	if ( numBytes == 0 ) {
		return qpU8String {};
	}

	qpU8String convertedString( numBytes + 1, '\0' );
	::WideCharToMultiByte( CP_UTF8, 0, string, length, reinterpret_cast< char * >( convertedString.Data() ), numBytes, NULL, NULL );
	return convertedString;
}

inline qpU8String qpWideToUTF8( const qpWideString & string ) {
	return qpWideToUTF8( string.c_str(), string.Length() );
}

qpWideString qpUTF8ToWide( const char c ) {
	return qpUTF8ToWide( &c, 1 );
}

qpU8String qpWideToUTF8( const wchar_t c ) {
	return qpWideToUTF8( &c, 1 );
}

#endif
