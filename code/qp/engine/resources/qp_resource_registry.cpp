#include "engine.pch.h"
#include "qp_resource_registry.h"
#include "loaders/qp_resource_loader.h"
#include "loaders/qp_tga_loader.h"
#include "qp/common/string/qp_string_util.h"

namespace {
	qpResourceLoader & GetResourceLoaderForPath( const qpFilePath & filePath ) {
		static qpImageLoader imageLoader;
		return imageLoader;
	}
}

qpResourceRegistry::~qpResourceRegistry() {
	for ( resourceEntry_t & entry : m_resourceEntries ) {
		ClearEntry( entry );
	}
}

const qpResource * qpResourceRegistry::LoadResource( const qpFilePath & filePath, const returnDefault_t defaultResource ) {
	m_lastError.Clear();

	if ( filePath.IsEmpty() ) {
		m_lastError = "Filepath can't be empty when loading resource";
		qpLog::Error( "qpResourceRegistry: %s!", m_lastError.c_str() );
		return NULL;
	}
	qpResourceLoader & resourceLoader = GetResourceLoaderForPath( filePath );

	qpResource * resource = FindMutable( filePath.c_str() );
	if ( resource != NULL ) {
		return resource;
	}
	resource = resourceLoader.LoadResource( filePath );
	resourceEntry_t entry;
	entry.resource = resource;
	entry.name = qpStringUtil::Duplicate( filePath.c_str() );
	CacheResource( entry );
	if ( resourceLoader.HasError() ) {
		qpLog::Error( R"(qpResourceRegistry: Resource "%s" has error: "%s")", filePath.c_str(), resourceLoader.GetLastError().c_str() );
		m_lastError = resourceLoader.GetLastError();
		if ( defaultResource == returnDefault_t::RETURN_NULL ) {
			return NULL;
		}
	}

	return resource;
}

bool qpResourceRegistry::SerializeResource( qpBinarySerializer & serializer, const qpResource * resource ) {
	int entryIndex = FindEntryIndexForResource( resource );
	QP_ASSERT( entryIndex != -1 );
	resourceEntry_t & entry = m_resourceEntries[ entryIndex ];
	return entry.resource->Serialize( serializer );
}

const qpResource * qpResourceRegistry::Find( const char * resourceName ) const {
	return FindMutable( resourceName );
}

qpResource * qpResourceRegistry::FindMutable( const char * resourceName ) const {
	for ( resourceEntry_t & entry : m_resourceEntries ) {
		if ( qpStrIcmp( entry.name, resourceName ) ) {
			return entry.resource;
		}
	}
	return NULL;
}

int qpResourceRegistry::FindEntryIndexForResource( const qpResource * resource ) const {
	for ( uint64 entryIndex = 0; entryIndex < m_resourceEntries.Length(); ++entryIndex ) {
		const resourceEntry_t & entry = m_resourceEntries[ static_cast< int >( entryIndex ) ];
		if ( entry.resource == resource ) {
			return qpVerifyStaticCast< int >( entryIndex );
		}
	}
	return -1;
}

void qpResourceRegistry::CacheResource( const resourceEntry_t & entry ) {
	QP_ASSERT( Find( entry.name ) == NULL );
	m_resourceEntries.Push( entry );
}

void qpResourceRegistry::ClearEntry( resourceEntry_t & entry ) {
	delete entry.resource;
	qpStringUtil::Free( entry.name );
	entry = resourceEntry_t();
}
