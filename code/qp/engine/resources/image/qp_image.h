#pragma once
#include "qp/engine/resources/qp_binary_serializer.h"
#include "qp/engine/resources/qp_resource.h"

enum class imageFormat_t : uint32 {
	UNKNOWN,
	R8G8B8A8
};

struct imageHeader_t {
	imageFormat_t format = imageFormat_t::UNKNOWN;
	int width = 0;
	int height = 0;
};

class qpImage : public qpResource {
public:
	qpImage() = default;
	qpImage( const imageHeader_t & header, byte * data ); // this takes ownership of the data
	virtual ~qpImage() override;

	virtual void MakeResourceDefault() override;
	virtual bool Serialize( qpBinarySerializer & serializer ) override;

	int GetWidth() const { return m_header.width; }
	int GetHeight() const { return m_header.height; }
	uint64 GetSize() const;

	const byte * GetData() const { return m_data; }

private:
	imageHeader_t m_header;
	byte * m_data = NULL;

};