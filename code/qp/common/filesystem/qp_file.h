#pragma once
#include "qp_file_path.h"
#include "qp/common/core/qp_macros.h"
#include "..\..\engine\debug\qp_log.h"
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
	QP_FILE_READ = QP_BIT( 0 ),
	QP_FILE_WRITE = QP_BIT( 1 ),
	QP_FILE_READ_WRITE = QP_FILE_READ | QP_FILE_WRITE
};

enum fileShareMode_t : uint32 {
	QP_FILE_SHARE_EXCLUSIVE = 0,
	QP_FILE_SHARE_READ = QP_BIT( 0 ),
	QP_FILE_SHARE_WRITE = QP_BIT( 1 ),
	QP_FILE_SHARE_DELETE = QP_BIT( 2 ),
	QP_FILE_SHARE_READ_WRITE = QP_FILE_SHARE_READ | QP_FILE_SHARE_WRITE
};

enum : uint64 { QP_FILE_FAILURE = ~0u };

/**
 * \brief A file for synchronous read/write.
 */
class qpFile {
public:
	qpFile() {}
	virtual ~qpFile() = default;

	bool Open( const qpFilePath & filePath, fileAccessMode_t accessMode );
	bool Open( const qpFilePath & filePath, fileAccessMode_t accessMode, fileShareMode_t shareMode );
	bool Read( qpList< byte > & buffer ) const;
	uint64 Read( void * buffer, const uint64 size ) const;
	bool Write( const qpList< byte > & buffer ) const;
	uint64 Write( const void * buffer, const uint64 size ) const;

	bool IsOpen() const;

	uint64  GetSize() const;
	void Close();

	fileAccessMode_t GetAccessMode() const { return m_accessMode; }
	fileShareMode_t GetShareMode() const { return m_shareMode; }
	qpFileHandle GetHandle() const { return m_handle;  }
	const qpFilePath & GetFilePath() const { return m_filePath; }
	bool GetExtension( qpString & outExtension ) const { return m_filePath.GetExtension( outExtension ); }
private:
	qpFileHandle m_handle = NULL;
	fileAccessMode_t m_accessMode = QP_FILE_METADATA;
	fileShareMode_t m_shareMode = QP_FILE_SHARE_EXCLUSIVE;
	qpFilePath m_filePath;
};

static inline qpList< byte > qpReadFile( const qpFilePath & filePath ) {
	qpList< byte > buffer;
	qpFile file;
	if( file.Open( filePath, fileAccessMode_t::QP_FILE_READ ) ) {
		if( !file.Read( buffer ) ) {
			qpLog::Error( "Failed to read file at %s.", filePath.c_str() );
		}
	} else {
		qpLog::Error( "Failed to open file at %s.", filePath.c_str() );
	}
	return buffer;
}