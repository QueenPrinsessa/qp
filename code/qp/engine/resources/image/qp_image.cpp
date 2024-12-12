#include "engine.pch.h"
#include "qp_image.h"
#include "qp/engine/resources/qp_binary_stream.h"

namespace qp {
	uint64 BitsPerPixelFromFormat( const imageFormat_t format ) {
		switch ( format ) {
			case imageFormat_t::R8G8B8A8: {
				return 8ull * 4ull;
			}
			case imageFormat_t::UNKNOWN:
			default:
				return 0ull;
		}
	}

	Image::~Image() {
		delete[] m_data;
	}
	
	void Image::MakeResourceDefault() {
	}
	
	Image::Image( const imageHeader_t & header, byte * data )
		: m_header( header ), m_data( data ) {}
	
	bool Image::Serialize( BinarySerializer & serializer ) {
		serializer.Serialize( m_header );
	
		const uint64 numBytes = GetSize();
		if ( serializer.IsReading() ) {
			delete[] m_data;
			m_data = new byte[ numBytes ];
		}
		serializer.SerializeBytes( m_data, numBytes );
	
		return true;
	}
	
	uint64 Image::GetSize() const {
		return ( BitsPerPixelFromFormat( m_header.format ) / 8ull ) * VerifyStaticCast< uint64 >( m_header.width ) * VerifyStaticCast< uint64 > ( m_header.height );
	}
}