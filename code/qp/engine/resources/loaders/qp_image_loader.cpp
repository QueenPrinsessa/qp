#include "engine.pch.h"
#include "qp_image_loader.h"
#include "qp_tga_loader.h"
#include "qp/engine/resources/image/qp_image.h"

namespace qp {
	Resource * ImageLoader::LoadResource_Internal( const File & file ) {
		const FilePath & path = file.GetFilePath();
		FilePath::stringType_t extension;
		path.GetExtension( extension );
		ResourceLoader * resourceLoader = GetImageLoaderFromExtension( extension );
		if ( resourceLoader == NULL ) {
			SetLastError( Format( "No suitable image loader found for extension: \"%s\".", extension.c_str() ) );
			return NULL;
		}
		if ( resourceLoader == this ) {
			Image * imageResource = new Image();
			DeserializeResourceFromFile( file, imageResource );
			return imageResource;
		}

		return resourceLoader->LoadResourceFromFile( file );
	}

	ResourceLoader * ImageLoader::GetImageLoaderFromExtension( const FilePath::stringType_t & ext ) {
		if ( ext == ".tga" ) {
			static TGALoader loader;
			return &loader;
		}
		if ( ext == ".qpimage" ) {
			return this;
		}

		return NULL;
	}
}