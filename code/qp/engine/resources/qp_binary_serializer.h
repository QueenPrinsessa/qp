#pragma once
#include "qp/common/containers/qp_list.h"

namespace qp {
	class File;

	enum class serializationMode_t {
		READING,
		WRITING
	};

	template < typename _type_ > requires( IsTrivialToCopy< _type_ > )
		struct serializeAsBinary_t {
		size_t operator()( const serializationMode_t state, const size_t offset, _type_ * inOutData, List< byte > & inOutBytes ) {
			constexpr size_t numBytes = sizeof( _type_ );
			QP_COMPILE_TIME_ASSERT( numBytes != 0 );
			if ( state == serializationMode_t::READING ) {
				QP_ASSERT( ( offset + numBytes ) <= inOutBytes.Length() );
				if ( ( offset + numBytes ) <= inOutBytes.Length() ) {
					CopyBytesUnchecked( inOutData, inOutBytes.Data() + offset, numBytes );
				}
			} else {
				inOutBytes.Resize( offset + numBytes );
				CopyBytesUnchecked( inOutBytes.Data() + offset, inOutData, numBytes );
			}
			return numBytes;
		}
	};

	struct serializeBytesParms_t {
		void * data = NULL;
		size_t numBytes = 0;
	};

	template <>
	struct serializeAsBinary_t< serializeBytesParms_t > {
		size_t operator()( const serializationMode_t state, const size_t offset, serializeBytesParms_t * inOutData, List< byte > & inOutBytes ) {
			serializeBytesParms_t & parms = *inOutData;
			QP_ASSERT( parms.numBytes != 0 );
			if ( state == serializationMode_t::READING ) {
				QP_ASSERT( ( offset + parms.numBytes ) <= inOutBytes.Length() );
				if ( ( offset + parms.numBytes ) <= inOutBytes.Length() ) {
					CopyBytesUnchecked( parms.data, inOutBytes.Data() + offset, parms.numBytes );
				}
			} else {
				inOutBytes.Resize( offset + parms.numBytes );
				CopyBytesUnchecked( inOutBytes.Data() + offset, parms.data, parms.numBytes );
			}
			return parms.numBytes;
		}
	};

	class BinarySerializer {
	public:
		BinarySerializer() = delete;
		virtual ~BinarySerializer() = default;

		template < typename _type_ >
		void Serialize( _type_ & inOutData ) {
			QP_ASSERT( !HasOverflowed() );
			m_offset += serializeAsBinary_t< _type_ >()( m_mode, m_offset, &inOutData, m_buffer );
		}

		void SerializeBytes( void * data, const size_t numBytes ) {
			serializeBytesParms_t parms { data, numBytes };
			Serialize( parms );
		}

		size_t GetOffset() const { return m_offset; }
		size_t GetBufferLength() const { return m_buffer.Length(); }
		size_t GetBufferCapacity() const { return m_buffer.Capacity(); }

		bool HasOverflowed() const { return m_offset > m_buffer.Length(); }

		bool IsReading() const { return m_mode == serializationMode_t::READING; }
		bool IsWriting() const { return m_mode == serializationMode_t::WRITING; }
	protected:
		BinarySerializer( const serializationMode_t state )
			: m_mode( state ) {
		}

		serializationMode_t	m_mode = serializationMode_t::READING;
		List< uint8_t > m_buffer;
		size_t m_offset = 0;
	};

	class BinaryReadSerializer : public BinarySerializer {
	public:
		BinaryReadSerializer( const void * buffer, const size_t numBytes )
			: BinarySerializer( serializationMode_t::READING ) {
			m_buffer.Resize( numBytes );
			CopyBytesUnchecked( m_buffer.Data(), buffer, m_buffer.Length() );
		}
		BinaryReadSerializer( const File & file );

		bool ReadAll() const { return m_offset == m_buffer.Length(); }
	};

	class BinaryWriteSerializer : public BinarySerializer {
	public:
		BinaryWriteSerializer()
			: BinarySerializer( serializationMode_t::WRITING ) {
		}

		void ReserveBytesToFit( const size_t numBytesToFit ) { m_buffer.Reserve( m_buffer.Length() + numBytesToFit ); }
		void FitBufferToOffset() { m_buffer.Resize( m_offset ); m_buffer.ShrinkToFit(); }
		const uint8_t * GetBuffer() const { return m_buffer.Data(); }
	};
}