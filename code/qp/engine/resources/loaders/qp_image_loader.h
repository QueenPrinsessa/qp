#pragma once
#include "qp_resource_loader.h"

namespace qp {
	class ImageLoader : public ResourceLoader {
	protected:
		virtual Resource * LoadResource_Internal( const File & file ) override;
	private:
		ResourceLoader * GetImageLoaderFromExtension( const FilePath::stringType_t & ext );
	};
}