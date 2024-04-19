#include "engine.pch.h"
#include "qp_resource_loader.h"
#include "qp/engine/resources/qp_resource.h"

static qpList< qpResource * > resources;

qpResource * qpResourceLoader::LoadResource( const qpFilePath & filePath ) {
	m_lastError.Clear();

	qpFile file;
	file.Open( filePath, fileAccessMode_t::QP_FILE_READ, fileShareMode_t::QP_FILE_SHARE_READ );
	if ( !file.IsOpen() ) {
		SetLastError( qpFormat( "Couldn't open file at path \"%s\".", filePath.c_str() ) );
		return NULL;
	}

	qpResource * resource = LoadResource_Internal( file );
	file.Close();

	QP_ASSERT( resource != NULL );
	if ( HasError() ) {
		resource->m_isDefault = true;
		resource->MakeResourceDefault();
	}
	resources.Emplace( resource );

	return resource;
}

void qpResourceLoader::DeleteAllResources() {
	for( qpResource * resource : resources ) {
		delete resource;
	}
}
