#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/math/qp_math.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_algorithms.h"

namespace qp {
	template < typename _type_ > class List;

	class BinaryStream {
	public:
		BinaryStream() {}
		~BinaryStream() { delete[] m_buffer; }

		template < typename _type_ >
		void WriteBinary( const _type_ & data ) {
			GrowIfNeededToFit( sizeof( _type_ ) );
			m_offset += CopyBytesUnchecked( m_buffer + m_offset, &data, sizeof( _type_ ) );
		}
		template < typename _type_ >
		void ReadBinary( _type_ & outData ) {
			m_offset += CopyBytesUnchecked( &outData, m_buffer + m_offset, sizeof( _type_ ) );
		}

		template < typename _type_ >
		void WriteElements( const _type_ * begin, const uint64 numElements ) {
			GrowIfNeededToFit( numElements * sizeof( _type_ ) );
			m_offset += CopyBytesUnchecked( m_buffer + m_offset, begin, sizeof( _type_ ) * numElements );
		}

		template < typename _type_ >
		void ReadElements( _type_ * begin, const uint64 numElements ) {
			m_offset += CopyBytesUnchecked( begin, m_buffer + m_offset, sizeof( _type_ ) * numElements );
		}

		template < typename _type_ >
		void ReadList( List< _type_ > & list ) {
			int length = 0;
			ReadBinary< _type_ >( length );
			list.Resize( length );
			ReadElements< _type_ >( list.Begin(), length );
		}

		template < typename _type_ >
		void WriteList( const List< _type_ > & list ) {
			GrowIfNeededToFit( sizeof( int ) + ( list.Length() * sizeof( _type_ ) ) );
			WriteBinary< _type_ >( list.Length() );
			WriteElements< _type_ >( list.Begin(), list.Length() );
		}

		void ConsumeBytes( const uint64  numBytes ) { m_offset += numBytes; }

		template < typename _type_ >
		void Consume() { ConsumeBytes( sizeof( _type_ ) ); }

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
			CopyBytesUnchecked( newData, m_buffer, m_offset );
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
				const uint64 newSize = static_cast< uint64 >( math::RoundToPow2< double >( static_cast< double >( m_size + numBytes ) ) );
				byte * newData = new byte[ newSize ] {};
				CopyBytesUnchecked( newData, m_buffer, m_size );
				delete[] m_buffer;
				m_buffer = newData;
				m_size = newSize;
			}

			QP_ASSERT_MSG( m_buffer != NULL, "Failed to allocate buffer." );
		}
	};
}