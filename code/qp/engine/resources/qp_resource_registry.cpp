#include "engine.pch.h"
#include "qp_resource_registry.h"
#include "loaders/qp_resource_loader.h"
#include "loaders/qp_tga_loader.h"
#include "qp/common/string/qp_string_util.h"

namespace qp {
	ResourceLoader & GetResourceLoaderForPath( const FilePath & filePath ) {
		static ImageLoader imageLoader;
		return imageLoader;
	}
	
	ResourceRegistry::~ResourceRegistry() {
		for ( resourceEntry_t & entry : m_resourceEntries ) {
			ClearEntry( entry );
		}
	}

	const Resource * ResourceRegistry::LoadResource( const FilePath & filePath, const returnDefault_t defaultResource ) {
		m_lastError.Clear();

		if ( filePath.IsEmpty() ) {
			m_lastError = "Filepath can't be empty when loading resource";
			debug::Error( "qpResourceRegistry: %s!", m_lastError.c_str() );
			return NULL;
		}
		ResourceLoader & resourceLoader = GetResourceLoaderForPath( filePath );

		Resource * resource = FindMutable( filePath.c_str() );
		if ( resource != NULL ) {
			return resource;
		}
		resource = resourceLoader.LoadResource( filePath );
		resourceEntry_t entry;
		entry.resource = resource;
		entry.name = string_util::Duplicate( filePath.c_str() );
		CacheResource( entry );
		if ( resourceLoader.HasError() ) {
			debug::Error( R"(qpResourceRegistry: Resource "%s" has error: "%s")", filePath.c_str(), resourceLoader.GetLastError().c_str() );
			m_lastError = resourceLoader.GetLastError();
			if ( defaultResource == returnDefault_t::RETURN_NULL ) {
				return NULL;
			}
		}

		return resource;
	}

	bool ResourceRegistry::SerializeResource( BinarySerializer & serializer, const Resource * resource ) {
		uint64 entryIndex = FindEntryIndexForResource( resource );
		QP_ASSERT( entryIndex != -1 );
		resourceEntry_t & entry = m_resourceEntries[ entryIndex ];
		return entry.resource->Serialize( serializer );
	}

	const Resource * ResourceRegistry::Find( const char * resourceName ) const {
		return FindMutable( resourceName );
	}

	Resource * ResourceRegistry::FindMutable( const char * resourceName ) const {
		for ( resourceEntry_t & entry : m_resourceEntries ) {
			if ( StrIcmp( entry.name, resourceName ) ) {
				return entry.resource;
			}
		}
		return NULL;
	}

	uint64 ResourceRegistry::FindEntryIndexForResource( const Resource * resource ) const {
		for ( uint64 entryIndex = 0; entryIndex < m_resourceEntries.Length(); ++entryIndex ) {
			const resourceEntry_t & entry = m_resourceEntries[ entryIndex ];
			if ( entry.resource == resource ) {
				return entryIndex;
			}
		}
		return 0xFFFFFFFF;
	}

	void ResourceRegistry::CacheResource( const resourceEntry_t & entry ) {
		QP_ASSERT( Find( entry.name ) == NULL );
		m_resourceEntries.Push( entry );
	}

	void ResourceRegistry::ClearEntry( resourceEntry_t & entry ) {
		delete entry.resource;
		string_util::Free( entry.name );
		entry = resourceEntry_t();
	}
}