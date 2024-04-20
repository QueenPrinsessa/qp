#include "engine.pch.h"
#include "qp_resource_loader.h"
#include "qp/engine/resources/qp_resource.h"

qpResource * qpResourceLoader::LoadResource( const qpFilePath & filePath ) {
	m_lastError.Clear();

	qpFile file;
	file.Open( filePath, fileAccessMode_t::QP_FILE_READ, fileShareMode_t::QP_FILE_SHARE_READ );
	if ( !file.IsOpen() ) {
		SetLastError( qpFormat( "Couldn't open file at path \"%s\".", filePath.c_str() ) );
		return NULL;
	}

	qpResource * resource = LoadResourceFromFile( file );
	file.Close();

	return resource;
}

qpResource * qpResourceLoader::LoadResourceFromFile( const qpFile & file ) {
	if ( !file.IsOpen() ) {
		SetLastError( "File wasn't open." );
		return NULL;
	}

	qpResource * resource = LoadResource_Internal( file );
	QP_ASSERT( resource != NULL );
	if ( HasError() ) {
		MakeResourceDefault( resource );
	}

	return resource;
}

void qpResourceLoader::DeserializeResourceFromFile( const qpFile & file, qpResource * resource ) {
	QP_ASSERT( resource != NULL );
	qpBinaryReadSerializer readSerializer( file );
	if ( !resource->Serialize( readSerializer ) ) {
		SetLastError( "Failed to deserialize resource." );
	}
	if ( !readSerializer.ReadAll() ) {
		SetLastError( qpFormat( "Didn't read all of resource. Read %ull / %ull bytes.", readSerializer.GetOffset(), readSerializer.GetBufferCapacity() ) );
	}
	if ( readSerializer.HasOverflowed() ) {
		SetLastError( qpFormat( "Overflowed while deserializing resource. Tried to read %ull / %ull bytes.", readSerializer.GetOffset(), readSerializer.GetBufferCapacity() ) );
	}
}

void qpResourceLoader::MakeResourceDefault( qpResource * resource ) {
	resource->m_isDefault = true;
	resource->MakeResourceDefault();
}
