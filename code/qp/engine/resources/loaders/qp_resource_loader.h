#pragma once
#include "qp/common/filesystem/qp_file.h"

class qpResource;

class qpResourceLoader {
public:
	virtual ~qpResourceLoader() = default;

	qpResource * LoadResource( const qpFilePath & filePath );
	qpResource * LoadResourceFromFile( const qpFile & file );

	bool HasError() const { return !m_lastError.IsEmpty(); }
	const qpString & GetLastError() const { return m_lastError; }

protected:
	virtual qpResource * LoadResource_Internal( const qpFile & file ) = 0; // file is guaranteed to be opened here
	void SetLastError( const qpString & err ) { m_lastError = err; }
	void DeserializeResourceFromFile( const qpFile & file, qpResource * resource );
private:
	qpString m_lastError;

	void MakeResourceDefault( qpResource * resource );
};
