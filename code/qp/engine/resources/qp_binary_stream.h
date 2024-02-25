#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/string/qp_string.h"
#include "qp/common/math/qp_math.h"

template < typename T > class qpList;

class qpBinaryStream {
public:
	qpBinaryStream() {}
	~qpBinaryStream() { delete[] m_buffer; }

	template < typename T >
	void WriteBinary( const T & data ) {
		GrowIfNeededToFit( sizeof( T ) );
		m_offset += qpCopyBytesUnchecked( m_buffer + m_offset, &data, sizeof( T ) );
	}
	template < typename T >
	void ReadBinary( T & outData ) {
		m_offset += qpCopyBytesUnchecked( &outData, m_buffer + m_offset, sizeof( T ) );
	}

	template < typename T >
	void WriteElements( const T * begin, const uint64 numElements ) {
		GrowIfNeededToFit( numElements * sizeof( T ) );
		m_offset += qpCopyBytesUnchecked( m_buffer + m_offset, begin, sizeof( T ) * numElements );
	}

	template < typename T >
	void ReadElements( T * begin, const uint64 numElements ) {
		m_offset += qpCopyBytesUnchecked( begin, m_buffer + m_offset, sizeof( T ) * numElements );
	}

	template < typename T >
	void ReadList( qpList< T > & list ) {
		int length = 0;
		ReadBinary< T >( length );
		list.Resize( length );
		ReadElements< T >( list.Begin(), length );
	}

	template < typename T >
	void WriteList( const qpList< T > & list ) {
		GrowIfNeededToFit( sizeof( int ) + ( list.Length() * sizeof( T ) ) );
		WriteBinary< T >( list.Length() );
		WriteElements< T >( list.Begin(), list.Length() );
	}

	void ConsumeBytes( const uint64  numBytes ) { m_offset += numBytes; }

	template < typename T >
	void Consume() { ConsumeBytes( sizeof( T ) ); }

	uint64 GetOffset() const { return m_offset; }

	void SetBuffer( byte * buffer, const uint64 size ) {
		QP_ASSERT( m_buffer != buffer );

		if ( m_ownsBuffer ) {
			delete[] m_buffer;
			m_size = 0;
			m_offset = 0;
			m_ownsBuffer = false;
		}

		if ( buffer == NULL ) {
			m_size = 0;
			m_offset = 0;
			m_ownsBuffer = true;
		}

		m_buffer = buffer;
		m_size = size;
	}

	void SetOffset( const uint64 offset ) {
		QP_ASSERT( offset <= m_size );
		m_offset = offset;
	}

	bool IsAtEndOfStream() const { return m_offset == m_size; }

	const byte * Buffer() const { return m_buffer; }

	void FitBuffer() {
		if ( m_buffer == NULL || m_size == m_offset || !m_ownsBuffer ) {
			return;
		}

		byte * newData = new byte[ m_offset ] {};
		qpCopyBytesUnchecked( newData, m_buffer, m_offset );
		delete[] m_buffer;
		m_buffer = newData;
		m_size = m_offset;

		QP_ASSERT_MSG( m_buffer != NULL, "Failed to allocate buffer." );
	}
private:
	byte * m_buffer = NULL;
	uint64 m_size = 0;
	uint64 m_offset = 0;
	bool m_ownsBuffer = true;

	void GrowIfNeededToFit( const uint64 numBytes ) {
		const bool needsToGrow = m_size < ( m_offset + numBytes );
		if ( !m_ownsBuffer ) {
			QP_ASSERT_MSG( needsToGrow, "buffer is too small" );
			return;
		}

		if ( needsToGrow ) {
			const uint64 newSize = static_cast< uint64 >( qpMath::RoundToPow2< double >( static_cast< double >( m_size + numBytes ) ) );
			byte *       newData = new byte[ newSize ] {};
			qpCopyBytesUnchecked( newData, m_buffer, m_size );
			delete[] m_buffer;
			m_buffer = newData;
			m_size = newSize;
		}

		QP_ASSERT_MSG( m_buffer != NULL, "Failed to allocate buffer." );
	}
};
