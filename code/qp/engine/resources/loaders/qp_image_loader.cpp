#include "engine.pch.h"
#include "qp_image_loader.h"
#include "qp_tga_loader.h"
#include "qp/engine/resources/image/qp_image.h"

qpResource * qpImageLoader::LoadResource_Internal( const qpFile & file ) {
	const qpFilePath & path = file.GetFilePath();
	qpFilePath::stringType_t extension; 
	path.GetExtension( extension );
	qpResourceLoader * resourceLoader = GetImageLoaderFromExtension( extension );
	if ( resourceLoader == NULL ) {
		SetLastError( qpFormat( "No suitable image loader found for extension: \"%s\".", extension.c_str() ) );
		return NULL;
	}
	if ( resourceLoader == this ) {
		qpImage * imageResource = new qpImage();
		DeserializeResourceFromFile( file, imageResource );
		return imageResource;
	}

	return resourceLoader->LoadResourceFromFile( file );
}

qpResourceLoader * qpImageLoader::GetImageLoaderFromExtension( const qpFilePath::stringType_t & ext ) {
	if ( ext == ".tga" ) {
		static qpTGALoader loader;
		return &loader;
	}
	if ( ext == ".qpimage" ) {
		return this;
	}

	return NULL;
}
