#pragma once
#include "qp_file_path.h"
#include "qp/common/core/qp_macros.h"
#include "qp/engine/debug/qp_debug.h"
#include "qp/common/containers/qp_list.h"
#include <cstddef>

class qpFileHandle {
public:
	qpFileHandle( void * handle ) : m_handle( handle ) {}
	operator void * () const { return m_handle; }
private:
	void * m_handle = NULL;
};

enum fileAccessMode_t : uint32 {
	QP_FILE_METADATA = 0,
	QP_FILE_READ = BIT( 0 ),
	QP_FILE_WRITE = BIT( 1 ),
	QP_FILE_READ_WRITE = QP_FILE_READ | QP_FILE_WRITE
};

enum fileShareMode_t : uint32 {
	QP_FILE_SHARE_EXCLUSIVE = 0,
	QP_FILE_SHARE_READ = BIT( 0 ),
	QP_FILE_SHARE_WRITE = BIT( 1 ),
	QP_FILE_SHARE_DELETE = BIT( 2 ),
	QP_FILE_SHARE_READ_WRITE = QP_FILE_SHARE_READ | QP_FILE_SHARE_WRITE
};

#define QP_FILE_FAILURE -1

/**
 * \brief A file for synchronous read/write.
 */
class qpFile {
public:
	qpFile() {}
	~qpFile() = default;

	bool Open( const qpFilePath & filePath, fileAccessMode_t accessMode );
	bool Open( const qpFilePath & filePath, fileAccessMode_t accessMode, fileShareMode_t shareMode );
	bool Read( qpList< byte > & buffer ) const;
	int Read( void * buffer, int size ) const;
	bool Write( const qpList< byte > & buffer ) const;
	int Write( const void * buffer, int size ) const; 

	bool IsOpen() const;

	int  GetSize() const;
	void Close();

	fileAccessMode_t GetAccessMode() const { return m_accessMode; }
	fileShareMode_t GetShareMode() const { return m_shareMode; }
	qpFileHandle GetHandle() const { return m_handle;  }
private:
	qpFileHandle m_handle = NULL;
	fileAccessMode_t m_accessMode = QP_FILE_METADATA;
	fileShareMode_t m_shareMode = QP_FILE_SHARE_EXCLUSIVE;
};

static inline qpList< byte > qpReadFile( const qpFilePath & filePath ) {
	qpList< byte > buffer;
	qpFile file;
	if( file.Open( filePath, fileAccessMode_t::QP_FILE_READ ) ) {
		if( !file.Read( buffer ) ) {
			qpDebug::Error( "Failed to read file at %s.", filePath.c_str() );
		}
	} else {
		qpDebug::Error( "Failed to open file at %s.", filePath.c_str() );
	}
	return buffer;
}