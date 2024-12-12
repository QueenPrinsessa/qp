#pragma once
#include "qp_utility.h"
#include "qp/common/core/qp_types.h"
#include "qp/common/core/qp_type_traits.h"

namespace qp {
	template < typename ... _types_ > requires ( ( sizeof ... ( _types_ ) > 0 ) ) && ( sizeof ... ( _types_ ) < UINT8_MAX )
		class Variant {
		enum { NUM_BYTES = SizeOfBiggestType< _types_... > };

		static inline uint8 s_nextTypeIndex = 0;
		template < typename _type_ >
		struct VariantTypeIndex {
			using type = _type_;
			static inline const uint8 index = s_nextTypeIndex++;
		};

		template < typename _type_ >
		static inline uint8 variantTypeIndex_t = VariantTypeIndex< _type_ >::index;

		template < typename _type_ >
		using variantType_t = typename VariantTypeIndex< _type_ >::type;
		public:
			constexpr Variant() {
				( []() {
					variantTypeIndex_t< _types_ >;
				}, ... );
			};
			constexpr ~Variant() {
				if ( m_hasData ) {
					DeconstructData();
				}
			}
			template < typename _type_, typename ... _args_ >
			_type_ & Emplace( _args_&&... args ) {
				static_assert( IsAnyOf< _type_, _types_... >, "Type is not part of parameter pack" );
				if ( m_hasData ) {
					DeconstructData();
					qpZeroMemory( m_data );
				}
				_type_ * newData = new ( &m_data[ 0 ] ) _type_( qpForward< _args_ >( args )... );
				m_hasData = true;
				m_index = VariantTypeIndex< _type_ >::index;
				return *newData;
			}

			template < typename _type_ >
			constexpr _type_ & GetValue() {
				static_assert( IsAnyOf< _type_, _types_... >, "Type is not part of parameter pack" );
				QP_ASSERT_MSG( HoldsType< _type_ >(), "qpVariant does not hold type, undefined behavior." );
				return *GetDataInternal< _type_ >();
			}

			template < typename _type_ >
			constexpr const _type_ & GetValue() const {
				static_assert( IsAnyOf< _type_, _types_... >, "Type is not part of parameter pack" );
				QP_ASSERT_MSG( HoldsType< _type_ >(), "qpVariant does not hold type, undefined behavior." );
				return GetDataInternal< _type_ >();
			}


			int GetIndex() const { return m_index; }
			template < typename _type_ >
			bool HoldsType() const { return m_hasData && ( m_index == variantTypeIndex_t< _type_ > ); }
		private:
			uint8 m_data[ NUM_BYTES ] {};
			uint8 m_index = 0;
			bool m_hasData = false;

			constexpr void DeconstructData() {
				( [ & ]() {
					if ( m_index == variantTypeIndex_t< _types_ > ) {
						variantType_t< _types_ > * data = reinterpret_cast< variantType_t< _types_ > * >( &m_data[ 0 ] );
						data->~type();
					}
				}( ), ... );
				m_hasData = false;
			}

			template < typename T >
			constexpr variantType_t< T > * GetDataInternal() {
				return reinterpret_cast< variantType_t< T > * >( &m_data[ 0 ] );
			}
	};
}