#pragma once
#include "qp_binary_stream.h"

enum class serializationMode_t {
	READING,
	WRITING
};

class qpBinarySerializer {
public:
	qpBinarySerializer( const serializationMode_t mode ) : m_mode( mode ) { }

	template < typename T >
	void SerializeBinary( T & data ) {
		if ( m_mode == serializationMode_t::WRITING ) {
			m_stream.WriteBinary( data );
		} else {
			m_stream.ReadBinary( data );
		}
	}

	template < typename T >
	void SerializeElements( T * begin, const uint64 numElements ) {
		if ( m_mode == serializationMode_t::WRITING ) {
			m_stream.WriteElements( begin, numElements );
		} else {
			m_stream.ReadElements( begin, numElements );
		}
	}

	template < typename T >
	void SerializeList( qpList< T > & list ) {
		if ( m_mode == serializationMode_t::WRITING ) {
			m_stream.WriteList( list );
		} else {
			m_stream.ReadElements( list );
		}
	}

	bool IsReading() const { return m_mode == serializationMode_t::READING; }
	bool IsWriting() const { return m_mode == serializationMode_t::WRITING; }

	const byte * GetData() const { return m_stream.Buffer(); }
	uint64 GetSize() const { return m_stream.GetOffset(); }
private:
	qpBinaryStream m_stream;
	serializationMode_t m_mode = serializationMode_t::READING;
};