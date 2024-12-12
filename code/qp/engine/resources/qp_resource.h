#pragma once
#include "qp/common/core/qp_intrusive_ref_ptr.h"
#include "qp_binary_serializer.h"

namespace qp {
	class Resource {
		friend class ResourceLoader;
	public:
		virtual ~Resource() = default;

		virtual void MakeResourceDefault() = 0;
		virtual bool Serialize( BinarySerializer & serializer ) = 0;
	private:
		bool m_isDefault = false;
	};
}