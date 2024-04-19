#pragma once
#include "qp/common/core/qp_intrusive_ref_ptr.h"
#include "qp_binary_serializer.h"

class qpResource {
	friend class qpResourceLoader;
public:
	virtual ~qpResource() = default;

	virtual void MakeResourceDefault() = 0;
	virtual bool Serialize( qpBinarySerializer & serializer ) = 0;
private:
	bool m_isDefault = false;
};
