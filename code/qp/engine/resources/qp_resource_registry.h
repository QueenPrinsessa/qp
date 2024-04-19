#pragma once
#include "qp/common/containers/qp_list.h"
#include "qp_resource.h"
#include "qp/common/filesystem/qp_file_path.h"
#include "qp/common/string/qp_string.h"

class qpBinarySerializer;
class qpResourceRegistry {
public:
	~qpResourceRegistry();

	const qpResource * Load( const qpFilePath & filePath );
	bool SerializeResource( qpBinarySerializer & serializer, const qpResource * resource );

	const qpResource * Find( const char * resourceName ) const;
private:
	struct resourceEntry_t {
		qpResource * resource = NULL;
		char * name = NULL;
	};
	qpList< resourceEntry_t > m_resourceEntries;

	qpResource * FindMutable( const char * resourceName ) const;
	int FindEntryIndexForResource( const qpResource * resource ) const;
	void CacheResource( const resourceEntry_t & entry );
	void ClearEntry( resourceEntry_t & entry );
};
