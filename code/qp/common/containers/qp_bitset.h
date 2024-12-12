#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp/common/utilities/qp_utility.h"

namespace qp {
	template< int _numBits_, typename _type_ = uint32 >
	class BitSet {
	public:
		static_assert( _numBits_ > 0, "Empty bitsets are not allowed." );
		enum {
			TYPE_BITS_SIZE = sizeof( _type_ ) * 8,
			DATA_COUNT = ( ( ( _numBits_ - 1 ) / TYPE_BITS_SIZE ) + 1 )
		};

		class Reference {
		public:
			Reference() = delete;
			Reference( BitSet * bitset, int pos ) : m_bitset( bitset ), m_pos( pos ) {}
			Reference( const Reference & other ) = default;

			Reference & operator=( const bool value ) { m_bitset->SetBit( m_pos, value ); return *this; }
			Reference & Toggle() { m_bitset->ToggleBit( m_pos ); return *this; }
			Reference & Set() { m_bitset->SetBit( m_pos ); return *this; }
			Reference & Clear() { m_bitset->ClearBit( m_pos ); return *this; }
			bool operator~() const { return !m_bitset->GetBit( m_pos ); }

			operator bool() const { return m_bitset->GetBit( m_pos ); }
		private:
			BitSet * m_bitset;
			int m_pos;
		};
		BitSet() = default;
		~BitSet() = default;
		BitSet( const BitSet & other );

		auto operator<=>( const BitSet & rhs ) const { return memcmp( m_data, rhs.m_data, DATA_COUNT ); }
		bool operator==( const BitSet & rhs ) const { return ( operator<=>( rhs ) == 0 ); }

		BitSet & operator=( const BitSet & rhs );

		bool operator[]( const int pos ) const { QP_ASSERT( pos < _numBits_ ); return QP_GET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
		Reference operator[]( const int pos ) { QP_ASSERT( pos < _numBits_ ); return qpReference( this, pos ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			bool operator[]( const _posType_ pos ) const { return operator[]( VerifyStaticCast< int >( pos ) ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			Reference operator[]( const _posType_ pos ) { return operator[]( VerifyStaticCast< int >( pos ) ); }
		BitSet operator~() { BitSet tmp = *this; tmp.ToggleAll(); return tmp; }

		void SetBit( const int pos ) { QP_ASSERT( pos < _numBits_ ); QP_SET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
		void SetBit( const int pos, const bool value ) { QP_ASSERT( pos < _numBits_ ); QP_SET_BIT_TO( m_data[ DataIndex( pos ) ], BitPos( pos ), value ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			void SetBit( const _posType_ pos ) { SetBit( VerifyStaticCast< int >( pos ) ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			void SetBit( const _posType_ pos, const bool value ) { SetBit( VerifyStaticCast< int >( pos ), value ); }

		void ClearBit( const int pos ) { QP_ASSERT( pos < _numBits_ ); QP_CLEAR_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			void ClearBit( const _posType_ pos ) { ClearBit( VerifyStaticCast< int >( pos ) ); }

		void ToggleBit( const int pos ) { QP_ASSERT( pos < _numBits_ ); QP_TOGGLE_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			void ToggleBit( const _posType_ pos ) { ToggleBit( VerifyStaticCast< int >( pos ) ); }

		void ToggleAll();
		void ClearAll();

		bool GetBit( const int pos ) const { QP_ASSERT( pos < _numBits_ ); return QP_GET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
		template < typename _posType_ > requires ( IsIntegral< _posType_ > || IsEnum< _posType_ > )
			bool GetBit( const _posType_ pos ) const { return GetBit( VerifyStaticCast< int >( pos ) ); }

		bool All() const;
		bool Any() const;
		bool None() const;

		int NumBits() const { return _numBits_; }

		BitSet & operator&( const BitSet & rhs );
		BitSet & operator|( const BitSet & rhs );
		BitSet & operator^( const BitSet & rhs );

		friend BitSet operator&( const BitSet & lhs, const BitSet & rhs );
		friend BitSet operator|( const BitSet & lhs, const BitSet & rhs );
		friend BitSet operator^( const BitSet & lhs, const BitSet & rhs );
	private:
		_type_ m_data[ DATA_COUNT ] {};
		int DataIndex( int n ) const { return ( n / TYPE_BITS_SIZE ); }
		int BitPos( int pos ) const { return pos % TYPE_BITS_SIZE; }
	};

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ >::BitSet( const BitSet & other ) {
		Copy( m_data, DATA_COUNT, other.m_data, DATA_COUNT );
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > & BitSet< _numBits_, _type_ >::operator=( const BitSet & rhs ) {
		Copy( m_data, DATA_COUNT, rhs.m_data, DATA_COUNT );
		return *this;
	}

	template< int _numBits_, typename _type_ >
	void BitSet< _numBits_, _type_ >::ToggleAll() {
		constexpr bool isEveryBitUsed = ( _numBits_ == ( DATA_COUNT * TYPE_BITS_SIZE ) );
		constexpr int fastToggleCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
		for ( int index = 0; index < fastToggleCount; index++ ) {
			m_data[ index ] = ~m_data[ index ];
		}

		if constexpr ( !isEveryBitUsed ) {
			constexpr int startBit = ( fastToggleCount * TYPE_BITS_SIZE );
			for ( int bit = startBit; bit < _numBits_; bit++ ) {
				ToggleBit( bit );
			}
		}
	}

	template< int _numBits_, typename _type_ >
	void BitSet< _numBits_, _type_ >::ClearAll() {
		constexpr bool isEveryBitUsed = ( _numBits_ == ( DATA_COUNT * TYPE_BITS_SIZE ) );
		constexpr int fastClearCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
		for ( int index = 0; index < ( DATA_COUNT - 1 ); index++ ) {
			m_data[ index ] = 0u;
		}
		if constexpr ( !isEveryBitUsed ) {
			constexpr int startBit = _numBits_ - ( fastClearCount * TYPE_BITS_SIZE );
			for ( int bit = startBit; bit < _numBits_; bit++ ) {
				ClearBit( bit );
			}
		}
	}

	template< int _numBits_, typename _type_ >
	bool BitSet< _numBits_, _type_ >::All() const {
		for ( int index = 0; index < DATA_COUNT; index++ ) {
			if ( m_data[ index ] != ~0u ) {
				return false;
			}
		}

		return true;
	}

	template< int _numBits_, typename _type_ >
	bool BitSet< _numBits_, _type_ >::Any() const {
		for ( int index = 0; index < DATA_COUNT; index++ ) {
			if ( m_data[ index ] & ~0u ) {
				return true;
			}
		}

		return false;
	}

	template< int _numBits_, typename _type_ >
	bool BitSet< _numBits_, _type_ >::None() const {
		for ( int index = 0; index < DATA_COUNT; index++ ) {
			if ( m_data[ index ] != 0u ) {
				return false;
			}
		}

		return true;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > & BitSet< _numBits_, _type_ >::operator&( const BitSet & rhs ) {
		constexpr bool isEveryBitUsed = ( _numBits_ == ( DATA_COUNT * TYPE_BITS_SIZE ) );
		constexpr int fastAndCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
		for ( int index = 0; index < fastAndCount; index++ ) {
			m_data[ index ] &= rhs.m_data[ index ];
		}
		if constexpr ( !isEveryBitUsed ) {
			constexpr int startBit = _numBits_ - ( fastAndCount * TYPE_BITS_SIZE );
			for ( int bit = startBit; bit < _numBits_; bit++ ) {
				if ( GetBit( bit ) && rhs.GetBit( bit ) ) {
					SetBit( bit );
				} else {
					ClearBit( bit );
				}
			}
		}
		return *this;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > & BitSet< _numBits_, _type_ >::operator|( const BitSet & rhs ) {
		constexpr bool isEveryBitUsed = ( _numBits_ == ( DATA_COUNT * TYPE_BITS_SIZE ) );
		constexpr int fastOrCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
		for ( int index = 0; index < fastOrCount; index++ ) {
			m_data[ index ] |= rhs.m_data[ index ];
		}
		if constexpr ( !isEveryBitUsed ) {
			constexpr int startBit = _numBits_ - ( fastOrCount * TYPE_BITS_SIZE );
			for ( int bit = startBit; bit < _numBits_; bit++ ) {
				if ( GetBit( bit ) || rhs.GetBit( bit ) ) {
					SetBit( bit );
				} else {
					ClearBit( bit );
				}
			}
		}
		return *this;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > & BitSet< _numBits_, _type_ >::operator^( const BitSet & rhs ) {
		constexpr bool isEveryBitUsed = ( _numBits_ == ( DATA_COUNT * TYPE_BITS_SIZE ) );
		constexpr int fastXorCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
		for ( int index = 0; index < fastXorCount; index++ ) {
			m_data[ index ] ^= rhs.m_data[ index ];
		}
		if constexpr ( !isEveryBitUsed ) {
			constexpr int startBit = _numBits_ - ( fastXorCount * TYPE_BITS_SIZE );
			for ( int bit = startBit; bit < _numBits_; bit++ ) {
				if ( ( static_cast< uint32 >( GetBit( bit ) ) + static_cast< uint32 >( rhs.GetBit( bit ) ) ) == 1 ) {
					SetBit( bit );
				} else {
					ClearBit( bit );
				}
			}
		}
		return *this;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > operator&( const BitSet< _numBits_, _type_ > & lhs, const BitSet< _numBits_, _type_ > & rhs ) {
		BitSet< _numBits_, _type_ > result = lhs;
		result &= rhs;
		return result;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > operator|( const BitSet< _numBits_, _type_ > & lhs, const BitSet< _numBits_, _type_ > & rhs ) {
		BitSet< _numBits_, _type_ > result = lhs;
		result |= rhs;
		return result;
	}

	template< int _numBits_, typename _type_ >
	BitSet< _numBits_, _type_ > operator^( const BitSet< _numBits_, _type_ > & lhs, const BitSet< _numBits_, _type_ > & rhs ) {
		BitSet< _numBits_, _type_ > result = lhs;
		result ^= rhs;
		return result;
	}
}