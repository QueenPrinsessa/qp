#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/string/qp_string.h"
#include "qp/common/platform/windows/qp_windows.h"

inline qpWideString qpUTF8ToWide( const qpString & string ) {
	int length = ::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0 );

	if ( length == 0 ) {
		return qpWideString {};
	}

	qpWideString convertedString( length + 1 );
	::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length );
	return convertedString;
}

inline qpString qpWideToUTF8( const qpWideString & string ) {
	int length = ::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0, NULL, NULL );

	if ( length == 0 ) {
		return qpString {};
	}

	qpString convertedString( string.Length() + 1 );
	::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length, NULL, NULL );
	return convertedString;
}

#endif