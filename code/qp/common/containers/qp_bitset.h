#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp/common/utilities/qp_utility.h"

template< int NUM_BITS, typename T = uint32 > 
class qpBitSet {
public:
	static_assert( NUM_BITS > 0, "Empty bitsets are not allowed." );
	enum {
		TYPE_BITS_SIZE = sizeof( T ) * 8,
		DATA_COUNT = ( ( ( NUM_BITS - 1 ) / TYPE_BITS_SIZE ) + 1 )
	};

	class qpReference {
	public:
		qpReference() = delete;
		qpReference( qpBitSet * bitset, int pos ): m_bitset( bitset ), m_pos( pos ) {}
		qpReference( const qpReference & other ) = default;

		qpReference & operator=( const bool value ) { m_bitset->SetBit( m_pos, value ); return *this; }
		qpReference & Toggle() { m_bitset->ToggleBit( m_pos ); return *this; }
		qpReference & Set() { m_bitset->SetBit( m_pos ); return *this; }
		qpReference & Clear() { m_bitset->ClearBit( m_pos ); return *this; }
		bool operator~() const { return !m_bitset->GetBit( m_pos ); }

		operator bool() const { return m_bitset->GetBit( m_pos ); }
	private:
		qpBitSet * m_bitset;
		int m_pos;
	};
	qpBitSet() = default;
	~qpBitSet() = default;
	qpBitSet( const qpBitSet & other );

	auto operator<=>( const qpBitSet & rhs ) const { return memcmp( m_data, rhs.m_data, DATA_COUNT ); }
	bool operator==( const qpBitSet & rhs ) const { return ( operator<=>( rhs ) == 0 ); }

	qpBitSet & operator=( const qpBitSet & rhs );
	
	bool operator[]( const int pos ) const { QP_ASSERT( pos < NUM_BITS ); return QP_GET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
	qpReference operator[]( const int pos ) { QP_ASSERT( pos < NUM_BITS ); return qpReference( this, pos ); }
	qpBitSet operator~() { qpBitSet tmp = *this; tmp.ToggleAll(); return tmp; }

	void SetBit( const int pos ) { QP_ASSERT( pos < NUM_BITS ); QP_SET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
	void SetBit( const int pos, const bool value ) { QP_ASSERT( pos < NUM_BITS ); QP_SET_BIT_TO( m_data[ DataIndex( pos ) ], BitPos( pos ), value ); }
	void ClearBit( const int pos ) { QP_ASSERT( pos < NUM_BITS ); QP_CLEAR_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
	void ToggleBit( const int pos ) { QP_ASSERT( pos < NUM_BITS ); QP_TOGGLE_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }
	void ToggleAll();
	void ClearAll();

	bool GetBit( const int pos ) const { QP_ASSERT( pos < NUM_BITS ); return QP_GET_BIT( m_data[ DataIndex( pos ) ], BitPos( pos ) ); }

	bool All() const;
	bool Any() const;
	bool None() const;

	int NumBits() const { return NUM_BITS; }

	qpBitSet & operator&( const qpBitSet & rhs );
	qpBitSet & operator|( const qpBitSet & rhs );
	qpBitSet & operator^( const qpBitSet & rhs );

	friend qpBitSet operator&( const qpBitSet & lhs, const qpBitSet & rhs );
	friend qpBitSet operator|( const qpBitSet & lhs, const qpBitSet & rhs );
	friend qpBitSet operator^( const qpBitSet & lhs, const qpBitSet & rhs );
private:
	T m_data[ DATA_COUNT ] {};
	int DataIndex( int n ) const { return ( n / TYPE_BITS_SIZE ); }
	int BitPos( int pos ) const { return pos % TYPE_BITS_SIZE; }
};

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T >::qpBitSet( const qpBitSet & other ) {
	qpCopy( m_data, DATA_COUNT, other.m_data, DATA_COUNT );
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > & qpBitSet< NUM_BITS, T >::operator=( const qpBitSet & rhs ) {
	qpCopy( m_data, DATA_COUNT, rhs.m_data, DATA_COUNT );
	return *this;
}

template< int NUM_BITS, typename T >
void qpBitSet< NUM_BITS, T >::ToggleAll() {
	constexpr bool isEveryBitUsed = ( NUM_BITS == ( DATA_COUNT * TYPE_BITS_SIZE ) );
	constexpr int fastToggleCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
	for( int index = 0; index < fastToggleCount; index++ ) {
		m_data[ index ] = ~m_data[ index ];
	}

	if constexpr ( !isEveryBitUsed ) {
		constexpr int startBit = ( fastToggleCount * TYPE_BITS_SIZE );
		for ( int bit = startBit; bit < NUM_BITS; bit++ ) {
			ToggleBit( bit );
		}
	}
}

template< int NUM_BITS, typename T >
void qpBitSet< NUM_BITS, T >::ClearAll() {
	constexpr bool isEveryBitUsed = ( NUM_BITS == ( DATA_COUNT * TYPE_BITS_SIZE ) );
	constexpr int fastClearCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
	for ( int index = 0; index < ( DATA_COUNT - 1 ); index++ ) {
		m_data[ index ] = 0u;
	}
	if constexpr ( !isEveryBitUsed ) {
		constexpr int startBit = NUM_BITS - ( fastClearCount * TYPE_BITS_SIZE );
		for ( int bit = startBit; bit < NUM_BITS; bit++ ) {
			ClearBit( bit );
		}
	}
}

template< int NUM_BITS, typename T >
bool qpBitSet< NUM_BITS, T >::All() const {
	for ( int index = 0; index < DATA_COUNT; index++ ) {
		if( m_data[ index ] != ~0u ) {
			return false;
		}
	}

	return true;
}

template< int NUM_BITS, typename T >
bool qpBitSet< NUM_BITS, T >::Any() const {
	for ( int index = 0; index < DATA_COUNT; index++ ) {
		if ( m_data[ index ] & ~0u ) {
			return true;
		}
	}

	return false;
}

template< int NUM_BITS, typename T >
bool qpBitSet< NUM_BITS, T >::None() const {
	for ( int index = 0; index < DATA_COUNT; index++ ) {
		if ( m_data[ index ] != 0u ) {
			return false;
		}
	}

	return true;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > & qpBitSet< NUM_BITS, T >::operator&( const qpBitSet & rhs ) {
	constexpr bool isEveryBitUsed = ( NUM_BITS == ( DATA_COUNT * TYPE_BITS_SIZE ) );
	constexpr int fastAndCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
	for ( int index = 0; index < fastAndCount; index++ ) {
		m_data[ index ] &= rhs.m_data[ index ];
	}
	if constexpr ( !isEveryBitUsed ) {
		constexpr int startBit = NUM_BITS - ( fastAndCount * TYPE_BITS_SIZE );
		for ( int bit = startBit; bit < NUM_BITS; bit++ ) {
			if( GetBit( bit ) && rhs.GetBit( bit ) ) {
				SetBit( bit );
			} else {
				ClearBit( bit );
			}
		}
	}
	return *this;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > & qpBitSet< NUM_BITS, T >::operator|( const qpBitSet & rhs ) {
	constexpr bool isEveryBitUsed = ( NUM_BITS == ( DATA_COUNT * TYPE_BITS_SIZE ) );
	constexpr int fastOrCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
	for ( int index = 0; index < fastOrCount; index++ ) {
		m_data[ index ] |= rhs.m_data[ index ];
	}
	if constexpr ( !isEveryBitUsed ) {
		constexpr int startBit = NUM_BITS - ( fastOrCount * TYPE_BITS_SIZE );
		for ( int bit = startBit; bit < NUM_BITS; bit++ ) {
			if ( GetBit( bit ) || rhs.GetBit( bit ) ) {
				SetBit( bit );
			} else {
				ClearBit( bit );
			}
		}
	}
	return *this;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > & qpBitSet< NUM_BITS, T >::operator^( const qpBitSet & rhs ) {
	constexpr bool isEveryBitUsed = ( NUM_BITS == ( DATA_COUNT * TYPE_BITS_SIZE ) );
	constexpr int fastXorCount = isEveryBitUsed ? DATA_COUNT : ( DATA_COUNT - 1 );
	for ( int index = 0; index < fastXorCount; index++ ) {
		m_data[ index ] ^= rhs.m_data[ index ];
	}
	if constexpr ( !isEveryBitUsed ) {
		constexpr int startBit = NUM_BITS - ( fastXorCount * TYPE_BITS_SIZE );
		for ( int bit = startBit; bit < NUM_BITS; bit++ ) {
			if ( ( static_cast< uint32 >( GetBit( bit ) ) + static_cast< uint32 >( rhs.GetBit( bit ) ) ) == 1 ) {
				SetBit( bit );
			} else {
				ClearBit( bit );
			}
		}
	}
	return *this;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > operator&( const qpBitSet< NUM_BITS, T > & lhs, const qpBitSet< NUM_BITS, T > & rhs ) {
	qpBitSet< NUM_BITS, T > result = lhs;
	result &= rhs;
	return result;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > operator|( const qpBitSet< NUM_BITS, T > & lhs, const qpBitSet< NUM_BITS, T > & rhs ) {
	qpBitSet< NUM_BITS, T > result = lhs;
	result |= rhs;
	return result;
}

template< int NUM_BITS, typename T >
qpBitSet< NUM_BITS, T > operator^( const qpBitSet< NUM_BITS, T > & lhs, const qpBitSet< NUM_BITS, T > & rhs ) {
	qpBitSet< NUM_BITS, T > result = lhs;
	result ^= rhs;
	return result;
}