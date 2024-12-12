#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/filesystem/qp_file.h"
#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
	DWORD FileAccessModeToWin32( fileAccessMode_t accessMode ) {
		DWORD winAccessMode = 0;

		if ( accessMode & fileAccessMode_t::QP_FILE_READ ) {
			winAccessMode |= FILE_GENERIC_READ;
		}
		if ( accessMode & fileAccessMode_t::QP_FILE_WRITE ) {
			winAccessMode |= FILE_GENERIC_WRITE;
		}

		return winAccessMode;
	}

	DWORD FileShareModeToWin32( fileShareMode_t shareMode ) {
		DWORD winShareMode = 0;

		if ( shareMode & fileShareMode_t::QP_FILE_SHARE_READ ) {
			winShareMode |= FILE_SHARE_READ;
		}
		if ( shareMode & fileShareMode_t::QP_FILE_SHARE_WRITE ) {
			winShareMode |= FILE_SHARE_WRITE;
		}
		if ( shareMode & fileShareMode_t::QP_FILE_SHARE_DELETE ) {
			winShareMode |= FILE_SHARE_DELETE;
		}

		return winShareMode;
	}

	bool File::Open( const FilePath & filePath, fileAccessMode_t accessMode ) {
		return Open( filePath, accessMode, QP_FILE_SHARE_EXCLUSIVE );
	}

	bool File::Open( const FilePath & filePath, fileAccessMode_t accessMode, fileShareMode_t shareMode ) {
		QP_ASSERT_MSG( m_handle == NULL, "Close the file before opening a new one!" );

		//Sys_CreateDirectory( filePath.c_str() );

		if ( m_handle == NULL ) {
			m_handle = ::CreateFileA( filePath.c_str(), FileAccessModeToWin32( accessMode ), FileShareModeToWin32( shareMode ), NULL, ( accessMode != fileAccessMode_t::QP_FILE_READ ) ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

			if ( m_handle == INVALID_HANDLE_VALUE ) {
				m_handle = NULL;
				return false;
			}

			m_accessMode = accessMode;
			m_shareMode = shareMode;
			m_filePath = filePath;

			return true;
		}

		return false;
	}

	bool File::Read( List< byte > & buffer ) const {
		const uint64 fileSize = GetSize();
		QP_ASSERT( fileSize != -1 );
		buffer.Resize( fileSize );
		return Read( buffer.Data(), buffer.Length() ) != QP_FILE_FAILURE;
	}

	uint64 File::Read( void * buffer, const uint64 size ) const {
		QP_ASSERT( m_handle != NULL );
		QP_ASSERT( m_accessMode & QP_FILE_READ || m_accessMode & QP_FILE_WRITE );
		DWORD bytesRead = 0;
		BOOL result = ::ReadFile( m_handle, buffer, VerifyStaticCast< DWORD >( size ), &bytesRead, NULL );
		if ( result == FALSE ) {
			return QP_FILE_FAILURE;
		}
		return bytesRead;
	}

	bool File::Write( const List< byte > & buffer ) const {
		return Write( buffer.Data(), buffer.Length() );
	}

	uint64 File::Write( const void * buffer, const uint64 size ) const {
		QP_ASSERT( m_handle != NULL );
		QP_ASSERT( m_accessMode & QP_FILE_WRITE );

		DWORD bytesWritten = 0;
		BOOL result = ::WriteFile( m_handle, buffer, VerifyStaticCast< DWORD >( size ), &bytesWritten, NULL );
		if ( result == FALSE ) {
			return QP_FILE_FAILURE;
		}
		return bytesWritten;
	}

	bool File::IsOpen() const {
		return m_handle != NULL;
	}

	uint64 File::GetSize() const {
		if ( m_handle == NULL ) {
			return QP_FILE_FAILURE;
		}
		LARGE_INTEGER size;
		if ( GetFileSizeEx( m_handle, &size ) == FALSE ) {
			return QP_FILE_FAILURE;
		}
		return VerifyStaticCast< uint64 >( size.QuadPart );
	}

	void File::Close() {
		if ( m_handle != NULL ) {
			::CloseHandle( m_handle );
			m_handle = NULL;
			m_accessMode = QP_FILE_METADATA;
			m_shareMode = QP_FILE_SHARE_EXCLUSIVE;
		}
	}
}
#endif
