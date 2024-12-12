#pragma once
#include "qp/common/filesystem/qp_file.h"

namespace qp {
	class Resource;

	class ResourceLoader {
	public:
		virtual ~ResourceLoader() = default;

		Resource * LoadResource( const FilePath & filePath );
		Resource * LoadResourceFromFile( const File & file );

		bool HasError() const { return !m_lastError.IsEmpty(); }
		const String & GetLastError() const { return m_lastError; }

	protected:
		virtual Resource * LoadResource_Internal( const File & file ) = 0; // file is guaranteed to be opened here
		void SetLastError( const String & err ) { m_lastError = err; }
		void DeserializeResourceFromFile( const File & file, Resource * resource );
	private:
		String m_lastError;

		void MakeResourceDefault( Resource * resource );
	};
}