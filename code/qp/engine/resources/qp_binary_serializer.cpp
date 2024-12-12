#include "engine.pch.h"
#include "qp_binary_serializer.h"
#include "qp/common/filesystem/qp_file.h"

namespace qp {
	BinaryReadSerializer::BinaryReadSerializer( const File & file )
		: BinarySerializer( serializationMode_t::READING ) {
		file.Read( m_buffer );
	}
}