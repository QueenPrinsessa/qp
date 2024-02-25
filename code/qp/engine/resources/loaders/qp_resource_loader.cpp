#include "engine.pch.h"
#include "qp_resource_loader.h"
#include "qp/engine/resources/qp_resource.h"

static qpList< qpResource * > resources;

const qpResource * qpResourceLoader::LoadResource( const qpFilePath & filePath ) {
	m_lastError.Clear();

	qpFile file;
	file.Open( filePath, fileAccessMode_t::QP_FILE_READ, fileShareMode_t::QP_FILE_SHARE_READ );
	if ( !file.IsOpen() ) {
		SetLastError( qpFormat( "Couldn't open file at path \"%s\".", filePath.c_str() ) );
		return NULL;
	}

	const qpResource * resource = LoadResource_Internal( file );
	file.Close();

	if ( resource != NULL ) {
		resources.Emplace( resource );
	}

	return resource;
}

void qpResourceLoader::DeleteAllResources() {
	for( qpResource * resource : resources ) {
		delete resource;
	}
}
