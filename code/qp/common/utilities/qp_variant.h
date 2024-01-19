#pragma once
#include "qp_algorithms.h"
#include "qp_utility.h"
#include "qp/common/core/qp_types.h"

template < typename ... ARGS > requires ( ( sizeof ... ( ARGS ) > 0 ) ) && ( sizeof ... ( ARGS ) < UINT8_MAX )
class qpVariant {
	enum { NUM_BYTES = qpSizeOfBiggestTypeValue< ARGS... > };

	static inline uint8 s_nextTypeIndex = 0;
	template < typename T >
	struct typeIndex_t {
		using type = T;
		static inline const uint8 s_index = s_nextTypeIndex++;
	};
public:
	constexpr qpVariant() {
		( [] () {
			typeIndex_t< ARGS >::s_index;
		}, ... );
	};
	constexpr ~qpVariant() {
		if ( m_hasData ) {
			DeconstructData();
		}
	}
	template < typename T, typename ... VARGS >
	T & Emplace( VARGS&&... vargs ) {
		static_assert( qpIsPartOfParameterPack< T, ARGS... >, "Type is not part of parameter pack" );
		if ( m_hasData ) {
			DeconstructData();
			qpZeroMemory( m_data );
		}
		T * newData = new ( &m_data[ 0 ] ) T( qpForward< VARGS >( vargs )... );
		m_hasData = true;
		m_index = typeIndex_t< T >::s_index;
		return *newData;
	}

	template < typename T >
	constexpr T & GetValue() {
		static_assert( qpIsPartOfParameterPack< T, ARGS... >, "Type is not part of parameter pack" );
		QP_ASSERT_MSG( HoldsType< T >(), "qpVariant does not hold type, undefined behavior." );
		return *GetDataInternal< T >();
	}

	template < typename T >
	constexpr const T & GetValue() const {
		static_assert( qpIsPartOfParameterPack< T, ARGS... >, "Type is not part of parameter pack" );
		QP_ASSERT_MSG( HoldsType< T >(), "qpVariant does not hold type, undefined behavior." );
		return GetDataInternal< T >();
	}


	int GetIndex() const { return m_index; }
	template < typename T >
	bool HoldsType() const { return m_hasData && ( m_index == typeIndex_t< T >::s_index ); }
private:
	uint8 m_data[ NUM_BYTES ] {};
	uint8 m_index = 0;
	bool m_hasData = false;

	constexpr void DeconstructData() {
		( [ & ] () {
			if ( m_index == typeIndex_t< ARGS >::s_index ) {
				using type = typename typeIndex_t< ARGS >::type;
				type * data = reinterpret_cast< type * >( &m_data[ 0 ] );
				data->~type();
			}
		}( ), ... );
		m_hasData = false;
	}

	template < typename T >
	constexpr typename typeIndex_t< T >::type * GetDataInternal() {
		return reinterpret_cast< typename typeIndex_t< T >::type * >( &m_data[ 0 ] );
	}
};

