#pragma once
#include "qp/common/allocation/qp_stack_allocator.h"
#include "qp/common/string/qp_string.h"
#include "qp/engine/debug/qp_log.h"

namespace qp {
	template < typename _type_ >
	class FilePathBase {
	public:
		using stringType_t = StringBase< _type_ >;

		FilePathBase() = default;
		FilePathBase( const _type_ * path );

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
	FilePathBase<_type_>::FilePathBase( const _type_ * path ) {
		m_path = path;
	}

	template< typename _type_ >
	bool FilePathBase<_type_>::GetExtension( stringType_t & outExtension ) const {
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
	bool FilePathBase<_type_>::GetExtension( _type_ * inOutBuffer, const int bufferLength, int & outExtensionLength ) const {
		if ( ( inOutBuffer != NULL ) && ( bufferLength == 0 ) ) {
			outExtensionLength = 0;
			debug::Error( "Empty buffer supplied for getting extension!" );
			return false;
		}

		auto extIt = m_path.ReverseFind( '.' );
		if ( extIt == m_path.End() ) {
			inOutBuffer[ 0 ] = CharTraits< _type_ >::NIL_CHAR;
			outExtensionLength = 0;
			return false;
		}
		const _type_ * ext = *extIt;
		const int extLength = StrLen( ext );
		if ( ( inOutBuffer == NULL ) || ( bufferLength < ( extLength + 1 ) ) ) {
			inOutBuffer = NULL;
			outExtensionLength = extLength;
			return false;
		}
		CopyBytes( inOutBuffer, VerifyStaticCast< uint64 >( bufferLength ) * sizeof( _type_ ), ext, VerifyStaticCast< uint64 >( extLength + 1 ) * sizeof( _type_ ) );
		outExtensionLength = extLength;
		return true;
	}

	using FilePath = FilePathBase< char >;
}