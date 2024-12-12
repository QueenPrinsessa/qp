#pragma once
#include "qp/common/core/qp_type_traits.h"

namespace qp {
	template < typename _type_ >
	QP_NO_DISCARD constexpr _type_ & Forward( removeReference_t< _type_ > & arg ) noexcept {
		return static_cast< _type_ & >( arg );
	}

	template < typename _type_ >
	QP_NO_DISCARD constexpr _type_ && Forward( removeReference_t< _type_ > && arg ) noexcept {
		return static_cast< _type_ && >( arg );
	}

	template < typename _type_ >
	QP_NO_DISCARD constexpr removeReference_t< _type_ > && Move( _type_ && arg ) noexcept {
		return static_cast< removeReference_t< _type_ > && >( arg );
	}


	template < typename ... _types_ >
	constexpr size_t SizeOfBiggestType() {
		size_t bytes = 0;
		( [ & ]() {
			if ( bytes < sizeof( _types_ ) ) {
				bytes = sizeof( _types_ );
			}
		}( ), ... );
		return bytes;
	}
}

#define QP_BIND_FUNCTION( func ) [ & ]( auto&& ... args ) -> decltype( auto ) { return this->func( qp::Forward< decltype( args ) >( args )... ); }