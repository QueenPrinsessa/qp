#pragma once
#include "qp_char_traits.h"
#include "qp/common/allocation/qp_allocation_util.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_utility.h"
#include "qp/common/math/qp_math.h"
#include "qp/engine/debug/qp_log.h"
#include <cstddef>
#include <ostream>
#include <compare>
#include <stdarg.h>
#include <type_traits>

template< typename _type_ = char >
static inline int qpStrLen( const _type_ * string ) {
	int length = 0;

	while ( *string != CharTraits< _type_ >::NIL_CHAR ) {
		++length;
		++string;
	}
	return length;
}

template < typename _type_ = char8_t >
static inline int qpStrUTF8CodePoints( const _type_ * string ) {
	int numCodePoints = 0;
	while ( *string != static_cast< _type_ >( 0 ) ) {
		if ( ( *string++ & 0xC0 ) != 0x80 ) {
			++numCodePoints;
		}
	}
	return numCodePoints;
}

template< typename _type_ = char >
static inline int qpStrCmp( const _type_ * a, const _type_ * b ) {
	if ( a == b ) {
		return 0;
	}
	using unsignedType = std::make_unsigned_t< _type_ >;
	const unsignedType * ua = reinterpret_cast< const unsignedType * > ( a );
	const unsignedType * ub = reinterpret_cast< const unsignedType * > ( b );

	while ( *ua && ( *ua == *ub ) ) {
		++ua;
		++ub;
	}
	return ( *ua > *ub ) - ( *ub > *ua );
}

template< typename _type_ = char >
static inline int qpStrIcmp( const _type_ * a, const _type_ * b ) {
	if ( a == b ) {
		return 0;
	}
	using unsignedType = std::make_unsigned_t< _type_ >;
	const unsignedType * ua = reinterpret_cast< const unsignedType * > ( a );
	const unsignedType * ub = reinterpret_cast< const unsignedType * > ( b );

	auto makeLowerCase = CharTraits< _type_ >::ToLower;
	while ( *ua && ( makeLowerCase( *ua ) == makeLowerCase( *ub ) ) ) {
		++ua;
		++ub;
	}
	return ( makeLowerCase( *ua ) > makeLowerCase( *ub ) ) - ( makeLowerCase( *ub ) > makeLowerCase( *ua ) );
}
template < typename _type_ = char >
static inline bool qpStrEmpty( const _type_ * str ) {
	return str == NULL || *str == CharTraits< _type_ >::NIL_CHAR;
}

template < typename _type_ = char >
static inline bool qpStrEquals( const _type_ * a, const _type_ * b ) {
	return ( qpStrCmp( a, b ) == 0 );
}

template< typename _type_, bool _allowAlloc_ = true, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
class qpStringBase {
public:
	using charTraits_t = CharTraits< _type_ >;
	using charType_t = _type_;
	static inline const stringEncoding_t STRING_ENCODING = ( ( _encoding_ == stringEncoding_t::DEFAULT ) ? charTraits_t::DEFAULT_STRING_ENCODING : _encoding_ );
	static inline const _type_ EMPTY_STRING [] { charTraits_t::NIL_CHAR };
	enum { STATIC_BUFFER_SIZE = 24 };
	struct Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = _type_;
		using pointer = _type_ *;
		using const_pointer = const _type_ *;

		Iterator( pointer ptr ) : m_ptr( ptr ) {}

		const_pointer operator *() const { return m_ptr; }
		pointer operator->() { return m_ptr; }

		Iterator & operator++() {
			m_ptr++;
			return *this;
		}
		Iterator operator++( int ) {
			Iterator it = m_ptr++;
			return it;
		}

		auto operator<=>( const qpStringBase::Iterator & ) const = default;
		bool operator==( const qpStringBase::Iterator & ) const = default;

	private:
		pointer m_ptr = NULL;
	};

	qpStringBase();
	explicit qpStringBase( const int capacity ) requires ( _allowAlloc_ );
	qpStringBase( const int length, const _type_ charToInsert ) requires ( _allowAlloc_ );
	qpStringBase( const _type_ c );
	qpStringBase( const _type_ * string );
	qpStringBase( const qpStringBase & other );
	qpStringBase( qpStringBase && rhs ) noexcept;
	~qpStringBase();

	qpStringBase & Assign( const _type_ * string, const int length );
	qpStringBase & Assign( const _type_ c );
	qpStringBase & Assign( const _type_ * string );
	qpStringBase & Assign( const qpStringBase & string );

	qpStringBase & Format( const _type_ * const format, ... );

	int Compare( const _type_ * string ) const;
	int Compare( const qpStringBase & string ) const;

	void Resize( const int newLength, const _type_ charToInsert );
	void Resize( const int newLength );
	void Reserve( const int requestedCapacity ) requires( _allowAlloc_ );
	void ShrinkToFit() requires( _allowAlloc_ );

	void Clear();
	void ClearAndFreeMemory() requires( _allowAlloc_ );
	void FreeMemory() requires( _allowAlloc_ );

	bool IsAllocated() const;

	_type_ & At( int index );
	const _type_ & At( int index ) const;

	int BufferLength() const { return m_length; }
	int Length() const { 
		if constexpr ( STRING_ENCODING == stringEncoding_t::UTF8 ) {
			return qpStrUTF8CodePoints( m_data ); 
		} else {
			return m_length; 
		}; 
	}
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0 || m_data[ 0 ] == '\0'; }

	stringEncoding_t GetEncoding() const { return STRING_ENCODING; }

	Iterator Find( const _type_ searchChar ) const;
	Iterator ReverseFind( const _type_ searchChar ) const;
	Iterator Find( const _type_ * searchStr ) const;
	Iterator ReverseFind( const _type_ * searchStr ) const;
	Iterator Find( const qpStringBase & searchStr ) const;
	Iterator ReverseFind( const qpStringBase & searchStr ) const;

	_type_ * Data() const { return m_data; }
	const _type_ * c_str() const { return m_data; }

	Iterator Begin() { return Iterator( &m_data[ 0 ] ); }
	Iterator End() { return Iterator( &m_data[ m_length ] ); }
	Iterator Begin() const { return Iterator( &m_data[ 0 ] ); }
	Iterator End() const { return Iterator( &m_data[ m_length ] ); }
	Iterator begin() { return Begin(); }
	Iterator end() { return End(); }
	Iterator begin() const { return Begin(); }
	Iterator end() const { return End(); }

	qpStringBase< _type_, _allowAlloc_, _encoding_ > & operator+=( const _type_ rhs );
	qpStringBase & operator+=( const _type_ * rhs );
	qpStringBase & operator+=( const qpStringBase & rhs );
	qpStringBase & operator=( const _type_ rhs );
	qpStringBase & operator=( const _type_ * rhs );
	qpStringBase & operator=( const qpStringBase & rhs );
	qpStringBase & operator=( qpStringBase && rhs ) noexcept;
	_type_ & operator[]( int index );
	const _type_ & operator[]( int index ) const;

	static const _type_ * Empty() { return EMPTY_STRING; }

	auto operator<=>( const qpStringBase & rhs ) const;
	auto operator<=>( const _type_ * rhs ) const;
	bool operator==( const qpStringBase & rhs ) const;
	bool operator==( const _type_ * rhs ) const;

	friend qpStringBase operator+( _type_ lhs, const qpStringBase & rhs ) {
		qpStringBase result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase operator+( const qpStringBase & lhs, _type_ rhs ) {
		qpStringBase result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase operator+( const _type_ * lhs, const qpStringBase & rhs ) {
		qpStringBase result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase operator+( const qpStringBase & lhs, const _type_ * rhs ) {
		qpStringBase result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase operator+( const qpStringBase & lhs, const qpStringBase & rhs ) {
		// makes sure there is only one allocation by allocating space for both at once
		qpStringBase result( lhs.m_length + rhs.m_length );
		result = lhs;
		result += rhs;
		return result;
	}

	friend std::basic_ostream< _type_ > & operator<<( std::basic_ostream< _type_ > & out, const qpStringBase & string ) {
		return out << string.m_data;
	}

private:
	int m_capacity = STATIC_BUFFER_SIZE;
	int m_length = 0;
	_type_ * m_data = m_staticBuffer;
	_type_ m_staticBuffer[ STATIC_BUFFER_SIZE ] {};
};

using qpString = qpStringBase< char >;
using qpWideString = qpStringBase< wchar_t >;

// u8 strings are not fully supported yet, expect odd behavior when using non-ascii u8 strings
using qpU8String = qpStringBase< char8_t, true, stringEncoding_t::UTF8 >;

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase() {}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( const int capacity )  requires ( _allowAlloc_ ) {
	QP_ASSERT_MSG( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( const int length, const _type_ charToInsert ) requires ( _allowAlloc_ ) {
	QP_ASSERT_MSG( length >= 0, "Length can't be less than 0." );
	Resize( length, charToInsert );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( _type_ c ) {
	Assign( c );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( const _type_ * string ) {
	Assign( string );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( const qpStringBase & other ) {
	Assign( other );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::qpStringBase( qpStringBase && rhs ) noexcept {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
	if ( rhs.IsAllocated() ) {
		m_data = rhs.m_data;
	} else {
		m_data = m_staticBuffer;
		QP_ASSERT( rhs.m_capacity == STATIC_BUFFER_SIZE );
		qpCopyBytes( m_data, STATIC_BUFFER_SIZE * sizeof( _type_ ), rhs.m_data, STATIC_BUFFER_SIZE * sizeof( _type_ ) );
	}

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;

	rhs.m_length = 0;
	rhs.m_capacity = 0;
	rhs.m_data = NULL;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ >::~qpStringBase() {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::Assign( const _type_ * string, const int length ) {
	if constexpr ( _allowAlloc_ ) {
		Reserve( length + 1 );
	} else {
		QP_ASSERT( ( length + 1 ) < m_capacity );
	}
	qpCopyBytesUnchecked( m_data, string, length * sizeof( _type_ ) );
	m_length = length;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::Assign( const _type_ c ) {
	return Assign( &c, sizeof( c ) );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::Assign( const _type_ * string ) {
	return Assign( string, qpStrLen( string ) );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::Assign( const qpStringBase & string ) {
	return Assign( string.m_data, string.m_length );
}

template<>
inline qpStringBase< char, true > & qpStringBase< char, true >::Format( const char * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );
	va_list args = NULL;
	va_start( args, format );
	int length = vsnprintf( NULL, 0, format, args );
	va_end( args );
	 
	Reserve( length + 1 );

	va_start( args, format );
	length = vsnprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template<>
inline qpStringBase< char, false > & qpStringBase< char, false >::Format( const char * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );

	va_list args = NULL;
	va_start( args, format );
	int length = vsnprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	if ( length < 0 ) {
		Clear();
	} else {
		m_length = length;
	}

	return *this;
}

template<>
inline qpStringBase< wchar_t, true > & qpStringBase< wchar_t >::Format( const wchar_t * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );

	va_list args = NULL;
	va_start( args, format );
	int length = _vsnwprintf( NULL, 0, format, args );
	va_end( args );

	Reserve( length + 1 );

	va_start( args, format );
	length = _vsnwprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template<>
inline qpStringBase< wchar_t, false > & qpStringBase< wchar_t, false >::Format( const wchar_t * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );

	va_list args = NULL;
	va_start( args, format );
	int length = _vsnwprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
int qpStringBase< _type_, _allowAlloc_, _encoding_ >::Compare( const _type_ * string ) const {
	return qpStrCmp< _type_ >( m_data, string );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
int qpStringBase< _type_, _allowAlloc_, _encoding_ >::Compare( const qpStringBase< _type_, _allowAlloc_, _encoding_ > & string ) const {
	return Compare( string.m_data );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_ >::Resize( const int newLength, const _type_ charToInsert ) {
	if ( m_length == newLength ) {
		return;
	}

	if constexpr ( _allowAlloc_ ) {
		Reserve( newLength + 1 );
	} else {
		QP_ASSERT( newLength < STATIC_BUFFER_SIZE );
	}

	int lengthDiff = newLength - m_length;

	if ( lengthDiff > 0 ) {
		qpSetMemory( m_data + ( m_length * sizeof( _type_ ) ), charToInsert, lengthDiff * sizeof( _type_ ) );
	} else if ( lengthDiff < 0 ) {
		qpSetMemory( m_data + ( ( static_cast< size_t >( m_length ) + lengthDiff ) * sizeof( _type_ ) ), 0, qpMath::Abs( lengthDiff ) * sizeof( _type_ ) );
	}

	m_length = newLength;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_ >::Resize( const int newLength ) {
	Resize( newLength, static_cast< _type_ >( 0 ) );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_ >::Reserve( const int requestedCapacity )  requires( _allowAlloc_ ) {
	if ( m_capacity >= requestedCapacity ) {
		return;
	}

	QP_ASSERT( requestedCapacity > STATIC_BUFFER_SIZE );

	int capacity = static_cast< int >( qpAllocationUtil::AlignSize( requestedCapacity, 8 * sizeof( _type_ ) ) );
	_type_ * newData = new _type_[ capacity ] { };
	qpCopyBytesUnchecked( newData, m_data, m_capacity * sizeof( _type_ ) );
	FreeMemory();
	m_data = newData;

	m_capacity = capacity;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_ >::ShrinkToFit()  requires( _allowAlloc_ ) {
	if ( !IsAllocated() ) {
		return;
	}
	int fittedCapacity = m_length + 1;
	if ( m_capacity == ( fittedCapacity ) ) {
		return;
	}

	_type_ * fittedData = ( fittedCapacity <= STATIC_BUFFER_SIZE ) ? m_staticBuffer : new _type_[ fittedCapacity ] {};
	qpCopyBytesUnchecked( fittedData, m_data, fittedCapacity * sizeof( _type_ ) );
	FreeMemory();
	m_data = fittedData;

	m_capacity = fittedCapacity;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_ >::Clear() {
	if ( m_data != NULL ) {
		m_data[ 0 ] = static_cast< _type_ >( 0 );
	}
	m_length = 0;
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase<_type_, _allowAlloc_, _encoding_>::ClearAndFreeMemory() requires ( _allowAlloc_ ) {
	Clear();
	FreeMemory();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
void qpStringBase<_type_, _allowAlloc_, _encoding_>::FreeMemory() requires ( _allowAlloc_ ) {
	if ( IsAllocated() ) {
		delete[] m_data;
	}
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
bool qpStringBase<_type_, _allowAlloc_, _encoding_>::IsAllocated() const {
	if constexpr ( _allowAlloc_ ) {
		return m_data != m_staticBuffer;
	} else {
		return false;
	}
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
_type_ & qpStringBase< _type_, _allowAlloc_, _encoding_ >::At( const int index ) {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
const _type_ & qpStringBase< _type_, _allowAlloc_, _encoding_ >::At( const int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_ >::Find( const _type_ searchChar ) const {
	const _type_ searchStr[ 2 ] {
		searchChar,
		charTraits_t::NIL_CHAR
	};
	return Find( searchStr );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_ >::ReverseFind( const _type_ searchChar ) const {
	const _type_ searchStr[ 2 ] {
		searchChar,
		charTraits_t::NIL_CHAR
	};
	return ReverseFind( searchStr );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_ >::Find( const _type_ * searchStr ) const {
	if ( qpStrEmpty( searchStr ) || IsEmpty() ) {
		return End();
	}

	int pos = 0;
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			posEnd = ++pos;
			searchPos = 0;
		}
	} while ( ( pos < m_length ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_ >::ReverseFind( const _type_ * searchStr ) const {
	if ( qpStrEmpty( searchStr ) || IsEmpty() ) {
		return End();
	}

	int searchStrLen = qpStrLen( searchStr );
	if ( searchStrLen > m_length ) {
		return End();
	} 
	if ( searchStrLen == m_length ) {
		if ( *this == searchStr ) {
			return Begin();
		}
		return End();
	}
	int pos = m_length - searchStrLen;
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			posEnd = --pos;
			searchPos = 0;
		} 
	} while ( ( ( posEnd - pos ) != searchStrLen ) && ( pos >= 0 ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase<_type_, _allowAlloc_, _encoding_>::Iterator qpStringBase<_type_, _allowAlloc_, _encoding_>::Find( const qpStringBase & searchStr ) const {
	if ( searchStr.IsEmpty() || IsEmpty() || ( searchStr.m_length > m_length ) ) {
		return End();
	}
	if ( searchStr.m_length == m_length ) {
		if ( *this == searchStr ) {
			return Begin();
		}
		return End()();
	}

	int pos = 0;
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			pos = posEnd;
			searchPos = 0;
		}
	} while ( ( pos < m_length ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
typename qpStringBase<_type_, _allowAlloc_, _encoding_>::Iterator qpStringBase<_type_, _allowAlloc_, _encoding_>::ReverseFind( const qpStringBase & searchStr ) const {
	if ( searchStr.IsEmpty() || IsEmpty() || ( searchStr.m_length > m_length ) ) {
		return End();
	}
	if ( searchStr.m_length == m_length ) {
		if ( *this == searchStr ) {
			return Begin();
		}
		return End();
	}

	int pos = m_length - searchStr.m_length;
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			posEnd = --pos;
			searchPos = 0;
		}
	} while ( ( ( posEnd - pos ) != searchStr.Length() ) && ( pos >= 0 ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator+=( const _type_ rhs ) {
	constexpr int rhsLength = 1;
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytesUnchecked( m_data + m_length, &rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator+=( const _type_ * rhs ) {
	const int rhsLength = qpStrLen( rhs );
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytesUnchecked( m_data + m_length, rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator+=( const qpStringBase< _type_, _allowAlloc_, _encoding_ > & rhs ) {
	Reserve( m_length + rhs.m_length + 1 );
	qpCopyBytesUnchecked( m_data + m_length, rhs.m_data, rhs.m_length * sizeof( _type_ ) );
	m_length += rhs.m_length;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator=( const _type_ rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator=( const _type_ * rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator=( const qpStringBase & rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _allowAlloc_, _encoding_ > & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator=( qpStringBase && rhs ) noexcept {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
	if ( rhs.IsAllocated() ) {
		m_data = rhs.m_data;
	} else {
		m_data = m_staticBuffer;
		QP_ASSERT( rhs.m_capacity == STATIC_BUFFER_SIZE );
		qpCopyBytes( m_data, STATIC_BUFFER_SIZE * sizeof( _type_ ), rhs.m_data, STATIC_BUFFER_SIZE * sizeof( _type_ ) );
	}

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;

	rhs.m_length = 0;
	rhs.m_capacity = 0;
	rhs.m_data = NULL;

	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
_type_ & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator[]( const int index ) {
	QP_ASSERT_MSG( index >= 0 && index < ( m_length + 1 ), "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
const _type_ & qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator[]( const int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < ( m_length + 1 ), "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
auto qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator<=>( const qpStringBase< _type_, _allowAlloc_, _encoding_ > & rhs ) const {
	return Compare( rhs );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
auto qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator<=>( const _type_ * rhs ) const {
	return Compare( rhs );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
bool qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator==( const qpStringBase & rhs ) const {
	return ( m_length == rhs.m_length ) && ( m_data[ 0 ] == rhs.m_data[ 0 ] ) && ( Compare( rhs ) == 0 );
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_ >
bool qpStringBase< _type_, _allowAlloc_, _encoding_ >::operator==( const _type_ * rhs ) const {
	return ( qpStrCmp< _type_ >( m_data, rhs ) == 0 );
}

template < typename... _args_ >
static inline qpString qpFormat( const char * const format, _args_&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpString formatted( 0 );
	formatted.Format( format, qpForward< _args_ >( args )... );
	return formatted;
}

template < typename... _args_ >
static inline qpU8String qpFormat( const char8_t * const format, _args_&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpU8String formatted( 0 );
	formatted.Format( format, qpForward< _args_ >( args )... );
	return formatted;
}

template < typename... _args_ >
static inline qpWideString qpFormat( const wchar_t * const format, _args_&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpWideString formatted( 0 );
	formatted.Format( format, qpForward< _args_ >( args )... );
	return formatted;
}

// defined per platform
extern qpWideString qpUTF8ToWide( const char * string, const int length );
extern qpWideString qpUTF8ToWide( const qpString & string );
extern qpWideString qpUTF8ToWide( const qpU8String & string );
extern qpU8String qpWideToUTF8( const wchar_t * string, const int length );
extern qpU8String qpWideToUTF8( const qpWideString & string );
extern qpWideString qpUTF8ToWide( const char c );
extern qpU8String qpWideToUTF8( const wchar_t c );
