#pragma once
#include "qp/common/core/qp_intrusive_ref_ptr.h"
#include "qp_binary_serializer.h"

class qpResource {
	QP_INTRUSIVE_REF_COUNTER;
public:
	virtual ~qpResource() = default;

	virtual bool SerializeToBinary( qpBinarySerializer & serializer ) = 0;
private:
};
