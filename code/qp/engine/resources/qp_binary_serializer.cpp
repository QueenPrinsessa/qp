#include "engine.pch.h"
#include "qp_binary_serializer.h"
#include "qp/common/filesystem/qp_file.h"

qpBinaryReadSerializer::qpBinaryReadSerializer( const qpFile & file )
	: qpBinarySerializer( serializationMode_t::READING ) {
	file.Read( m_buffer );
}
