#pragma once
#include "qp_image_loader.h"

namespace qp {
	class TGALoader : public ImageLoader {
	protected:
		virtual Resource * LoadResource_Internal( const File & file ) override;
	private:
	};
}