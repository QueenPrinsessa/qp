#include "engine.pch.h"
#include "qp_image.h"
#include "qp/engine/resources/qp_binary_stream.h"

static uint64 BitsPerPixelFromFormat( const imageFormat_t format ) {
	switch ( format ) {
		case imageFormat_t::R8G8B8A8: {
			return 8ull * 4ull;
		}
		case imageFormat_t::UNKNOWN:
		default:
			return 0ull;
	}
}

qpImage::~qpImage() {
	delete[] m_data;
}

qpImage::qpImage( const imageHeader_t & header, byte * data )
	: m_header( header ), m_data( data ) {}

bool qpImage::SerializeToBinary( qpBinarySerializer & serializer ) {
	serializer.SerializeBinary( m_header );

	const uint64 numBytes = GetSize();
	if ( serializer.IsReading() ) {
		delete[] m_data;
		m_data = new byte[ numBytes ];
	}
	serializer.SerializeElements( m_data, numBytes );

	return true;
}

uint64 qpImage::GetSize() const {
	return ( BitsPerPixelFromFormat( m_header.format ) / 8ull ) * m_header.width * m_header.height;
}
