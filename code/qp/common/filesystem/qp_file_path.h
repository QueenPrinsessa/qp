#pragma once
#include "qp/common/allocation/qp_stack_allocator.h"
#include "qp/common/string/qp_string.h"
#include "qp/engine/debug/qp_log.h"

// todo: create an actual class for file paths, for now just a string will do.
//using qpFilePath = qpString;

template < typename _type_ >
class qpFilePathBase {
public:
	using stringType_t = qpStringBase< _type_ >;

	qpFilePathBase() = default;
	qpFilePathBase( const _type_ * path );

	bool GetExtension( stringType_t & outExtension ) const;
	
	// outExtensionLength returns the length of the extension without its null terminator
	// returns whether the extension was copied into the provided buffer
	bool GetExtension( _type_ * inOutBuffer, const int bufferLength, int & outExtensionLength ) const;

	bool IsEmpty() const { return m_path.IsEmpty(); }
	const _type_ * c_str() const { return m_path.c_str(); }
private:
	stringType_t m_path;
};

template< typename _type_ >
qpFilePathBase<_type_>::qpFilePathBase( const _type_ * path ) {
	m_path = path;
}

template< typename _type_ >
bool qpFilePathBase<_type_>::GetExtension( stringType_t & outExtension ) const {
	const int bufferLength = 256;
	_type_ buffer[ bufferLength ];
	int extensionLength = 0;
	if ( GetExtension( buffer, bufferLength, extensionLength ) ) {
		outExtension.Assign( buffer, extensionLength );
		return true;
	}
	QP_ASSERT( bufferLength <= QP_ARRAY_LENGTH( buffer ) );
	return false;
}

template< typename _type_ >
bool qpFilePathBase<_type_>::GetExtension( _type_ * inOutBuffer, const int bufferLength, int & outExtensionLength ) const {
	if ( ( inOutBuffer != NULL ) && ( bufferLength == 0 ) ) {
		outExtensionLength = 0;
		qpDebug::Error( "Empty buffer supplied for getting extension!" );
		return false;
	}

	auto extIt = m_path.ReverseFind( '.' );
	if ( extIt == m_path.End() ) {
		inOutBuffer[ 0 ] = CharTraits< _type_ >::NIL_CHAR;
		outExtensionLength = 0;
		return false;
	}
	const _type_ * ext = *extIt;
	const int extLength = qpStrLen( ext );
	if ( ( inOutBuffer == NULL ) || ( bufferLength < ( extLength + 1 ) ) ) {
		inOutBuffer = NULL;
		outExtensionLength = extLength;
		return false;
	}
	qpCopyBytes( inOutBuffer, qpVerifyStaticCast< uint64 >( bufferLength ) * sizeof( _type_ ), ext, qpVerifyStaticCast< uint64 >( extLength + 1 ) * sizeof( _type_ ) );
	outExtensionLength = extLength;
	return true;
}

using qpFilePath = qpFilePathBase< char >;