#pragma once
#include "qp/common/containers/qp_list.h"
#include "qp_resource.h"
#include "qp/common/filesystem/qp_file_path.h"
#include "qp/common/string/qp_string.h"

namespace qp {
	// todo: remove returnDefault_t when there is a way to check the resource for error instead.
	enum class returnDefault_t {
		RETURN_DEFAULT,
		RETURN_NULL
	};

	class BinarySerializer;
	class ResourceRegistry {
	public:
		~ResourceRegistry();

		const Resource * LoadResource( const FilePath & filePath, const returnDefault_t defaultResource );
		bool SerializeResource( BinarySerializer & serializer, const Resource * resource );

		const Resource * Find( const char * resourceName ) const;

		bool HasResourceError() const { return !m_lastError.IsEmpty(); }
		const String & GetLastResourceError() const { return m_lastError; }
	private:
		struct resourceEntry_t {
			Resource * resource = NULL;
			char * name = NULL;
		};
		List< resourceEntry_t > m_resourceEntries;
		String m_lastError;

		Resource * FindMutable( const char * resourceName ) const;
		int FindEntryIndexForResource( const Resource * resource ) const;
		void CacheResource( const resourceEntry_t & entry );
		void ClearEntry( resourceEntry_t & entry );
	};
}