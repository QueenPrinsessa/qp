#include "engine.pch.h"
#include "qp_tga_loader.h"
#include "qp/engine/resources/qp_binary_stream.h"
#include "qp/engine/resources/image/qp_image.h"

namespace qp {
	// http://www.paulbourke.net/dataformats/tga/
	Resource * TGALoader::LoadResource_Internal( const File & file ) {
		List< byte > buffer;
		file.Read( buffer );

		BinaryStream stream;
		stream.SetBuffer( buffer.Data(), buffer.Length() );

		enum dataType_t : char {
			NO_IMAGE_DATA = 0,
			UNCOMPRESSED_COLOR_MAPPED = 1,
			UNCOMPRESSED_RGB = 2,
			UNCOMPRESSED_BW = 3,
			RUNLENGTH_ENCODED_COLOR_MAPPED = 9,
			RUNLENGTH_ENCODED_RGB = 10,
			COMPRESSED_BW = 11,
			COMPRESSED_COLOR_MAPPED = 32,
			COMPRESSED_COLOR_MAPPED_QUADTREE = 33
		};

#pragma pack( push )
#pragma pack( 1 )
		struct tgaHeader_t {
			char idLength;
			char colorMapType;
			dataType_t dataTypeCode;
			short colorMapOrigin;
			short colourMapLength;
			char colorMapDepth;
			short int xOrigin;
			short int yOrigin;
			short width;
			short height;
			char bitsPerPixel;
			char imageDescriptor;
		};
#pragma pack( pop )
		static_assert( sizeof( tgaHeader_t ) == 18, "TGA Header must be 18 bytes as per the specification" );

		tgaHeader_t header {};
		stream.ReadBinary< tgaHeader_t >( header );

		if ( header.dataTypeCode != dataType_t::UNCOMPRESSED_RGB || header.bitsPerPixel != 32 ) {
			SetLastError( "Unsupported TGA type. Currently only uncompressed 32bpp RGB is supported." );
			return new Image();
		}

		const uint64 pixelByteSize = static_cast< uint64 >( header.bitsPerPixel / 8 );
		const uint64 numBytes = pixelByteSize * VerifyStaticCast< uint64 >( header.width ) * VerifyStaticCast< uint64 >( header.height );
		byte * data = new byte[ numBytes ];
		stream.ReadElements( data, numBytes );

		// TARGA stores its images in B8G8R8A8 while we want the image in R8G8B8A8
		// so we need to swap the blue and red channels.
		for ( uint64 row = 0; row < VerifyStaticCast< uint64 >( header.height ); ++row ) {
			const uint64 rowStart = ( row * ( sizeof( byte ) * pixelByteSize ) * VerifyStaticCast< uint64 >( header.width ) );
			for ( uint64 x = 0; x < VerifyStaticCast< uint64 >( header.width ); ++x ) {
				Swap( data[ rowStart + x * pixelByteSize ],
						data[ rowStart + x * pixelByteSize + 2 ] );
			}
		}

		imageHeader_t imageHeader;
		imageHeader.format = imageFormat_t::R8G8B8A8;
		imageHeader.width = header.width;
		imageHeader.height = header.height;
		return new Image( imageHeader, data );
	}
}