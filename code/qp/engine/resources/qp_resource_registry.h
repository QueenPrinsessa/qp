#pragma once
#include "qp/common/containers/qp_list.h"
#include "qp_resource.h"
#include "qp/common/filesystem/qp_file_path.h"
#include "qp/common/string/qp_string.h"

// todo: remove returnDefault_t when there is a way to check the resource for error instead.
enum class returnDefault_t {
	RETURN_DEFAULT,
	RETURN_NULL
};

class qpBinarySerializer;
class qpResourceRegistry {
public:
	~qpResourceRegistry();

	const qpResource * LoadResource( const qpFilePath & filePath, const returnDefault_t defaultResource );
	bool SerializeResource( qpBinarySerializer & serializer, const qpResource * resource );

	const qpResource * Find( const char * resourceName ) const;

	bool HasResourceError() const { return !m_lastError.IsEmpty(); }
	const qpString & GetLastResourceError() const { return m_lastError; }
private:
	struct resourceEntry_t {
		qpResource * resource = NULL;
		char * name = NULL;
	};
	qpList< resourceEntry_t > m_resourceEntries;
	qpString m_lastError;

	qpResource * FindMutable( const char * resourceName ) const;
	int FindEntryIndexForResource( const qpResource * resource ) const;
	void CacheResource( const resourceEntry_t & entry );
	void ClearEntry( resourceEntry_t & entry );
};
