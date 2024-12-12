#include "engine.pch.h"
#include "qp_resource_loader.h"
#include "qp/engine/resources/qp_resource.h"

namespace qp {
	Resource * ResourceLoader::LoadResource( const FilePath & filePath ) {
		m_lastError.Clear();

		File file;
		file.Open( filePath, fileAccessMode_t::QP_FILE_READ, fileShareMode_t::QP_FILE_SHARE_READ );
		if ( !file.IsOpen() ) {
			SetLastError( Format( "Couldn't open file at path \"%s\".", filePath.c_str() ) );
			return NULL;
		}

		Resource * resource = LoadResourceFromFile( file );
		file.Close();

		return resource;
	}

	Resource * ResourceLoader::LoadResourceFromFile( const File & file ) {
		if ( !file.IsOpen() ) {
			SetLastError( "File wasn't open." );
			return NULL;
		}

		Resource * resource = LoadResource_Internal( file );
		QP_ASSERT( resource != NULL );
		if ( HasError() ) {
			MakeResourceDefault( resource );
		}

		return resource;
	}

	void ResourceLoader::DeserializeResourceFromFile( const File & file, Resource * resource ) {
		QP_ASSERT( resource != NULL );
		BinaryReadSerializer readSerializer( file );
		if ( !resource->Serialize( readSerializer ) ) {
			SetLastError( "Failed to deserialize resource." );
		}
		if ( !readSerializer.ReadAll() ) {
			SetLastError( Format( "Didn't read all of resource. Read %ull / %ull bytes.", readSerializer.GetOffset(), readSerializer.GetBufferCapacity() ) );
		}
		if ( readSerializer.HasOverflowed() ) {
			SetLastError( Format( "Overflowed while deserializing resource. Tried to read %ull / %ull bytes.", readSerializer.GetOffset(), readSerializer.GetBufferCapacity() ) );
		}
	}

	void ResourceLoader::MakeResourceDefault( Resource * resource ) {
		resource->m_isDefault = true;
		resource->MakeResourceDefault();
	}
}