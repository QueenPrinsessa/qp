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

template < typename _type_ = char8_t, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT>
static inline bool qpStrIsCodePoint( const _type_ c ) {
	constexpr stringEncoding_t encoding = ( _encoding_ == stringEncoding_t::DEFAULT ) ? CharTraits< _type_ >::DEFAULT_STRING_ENCODING : _encoding_;
	if constexpr ( encoding == stringEncoding_t::UTF8 ) {
		return ( ( c & 0xC0 ) != 0x80 );
	} else {
		return true;
	}
}

enum class searchDirection_t {
	FORWARD,
	BACKWARD
};
template < typename _type_ = char8_t, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
static inline int qpStrFindNextCodePoint( const _type_ * str, const int strLen, const int startPos, const searchDirection_t searchDir ) {
	QP_ASSERT( startPos >= 0 && startPos < strLen );
	QP_ASSERT( str != NULL );
	constexpr stringEncoding_t encoding = ( _encoding_ == stringEncoding_t::DEFAULT ) ? CharTraits< _type_ >::DEFAULT_STRING_ENCODING : _encoding_;
	const int direction = ( searchDir == searchDirection_t::FORWARD ) ? 1 : -1;
	int pos = startPos;
	if constexpr ( encoding == stringEncoding_t::UTF8 ) {
		do {
			pos += direction;
			if ( ( pos == strLen ) || ( pos < 0 ) ) {
				return pos;
			}
			if ( qpStrIsCodePoint< _type_, encoding >( str[ pos ] ) ) {
				return pos;
			}
		} while ( ( pos >= 0 ) && ( pos < strLen ) && ( str[ pos ] != CharTraits< _type_ >::NIL_CHAR ) );
	} else {
		pos += direction;
	}
	return pos;
}
template < typename _type_ = char8_t, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
static inline int qpStrFindNextCodePoint( const _type_ * str, const int startPos, const searchDirection_t searchDir ) {
	QP_ASSERT( startPos >= 0 );
	QP_ASSERT( str != NULL );
	QP_ASSERT( str[ startPos ] != CharTraits< _type_ >::NIL_CHAR );
	constexpr stringEncoding_t encoding = ( _encoding_ == stringEncoding_t::DEFAULT ) ? CharTraits< _type_ >::DEFAULT_STRING_ENCODING : _encoding_;
	const int direction = ( searchDir == searchDirection_t::FORWARD ) ? 1 : -1;
	int pos = startPos;
	do {
		pos += direction;
		if ( ( str[ pos ] == CharTraits< _type_ >::NIL_CHAR ) || ( pos < 0 ) ) {
			return pos;
		}
		if ( qpStrIsCodePoint< _type_, encoding >( str[ pos ] ) ) {
			return pos;
		}
	} while ( ( pos >= 0 ) && ( str[ pos ] != CharTraits< _type_ >::NIL_CHAR ) );
	return pos;
}
template < typename _type_ = char8_t, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
static inline int qpStrCodePoints( const _type_ * string ) {
	constexpr stringEncoding_t encoding = ( _encoding_ == stringEncoding_t::DEFAULT ) ? CharTraits< _type_ >::DEFAULT_STRING_ENCODING : _encoding_;
	int numCodePoints = 0;
	while ( *string != CharTraits< _type_ >::NIL_CHAR ) {
		if ( qpStrIsCodePoint< _type_, encoding >( *string++ ) ) {
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

template< typename _type_, bool _allowAlloc_ = true, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT, uint32 _staticBufferCapacity_ = 24u >
class qpStringBase {
public:
	using charTraits_t = CharTraits< _type_ >;
	using charType_t = _type_;
	static inline const stringEncoding_t STRING_ENCODING = ( ( _encoding_ == stringEncoding_t::DEFAULT ) ? charTraits_t::DEFAULT_STRING_ENCODING : _encoding_ );
	static inline const _type_ EMPTY_STRING [] { charTraits_t::NIL_CHAR };
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

	void Resize( const int requestedLength, const _type_ charToInsert );
	void Resize( const int requestedLength );
	void Reserve( const int requestedCapacity ) requires( _allowAlloc_ );
	void ShrinkToFit() requires( _allowAlloc_ );

	void Clear();
	void ClearAndFreeMemory();
	void FreeMemory() requires( _allowAlloc_ );

	bool IsAllocated() const;

	_type_ & At( int index );
	const _type_ & At( int index ) const;

	int DataLength() const { return m_length; } // returns num chars in data
	int Length() const;	// returns number of code points. same as num chars for non-utf8 strings
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0 || m_data[ 0 ] == charTraits_t::NIL_CHAR; }

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

	qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & operator+=( const _type_ rhs );
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
	int m_capacity = _staticBufferCapacity_;
	int m_length = 0;
	_type_ * m_data = m_staticBuffer;
	_type_ m_staticBuffer[ _staticBufferCapacity_ ] {};
};

using qpString = qpStringBase< char >;
using qpWideString = qpStringBase< wchar_t >;

// u8 strings are not fully supported yet, expect odd behavior when using non-ascii u8 strings
using qpU8String = qpStringBase< char8_t, true, stringEncoding_t::UTF8 >;

template < typename _type_, uint32 _capacity_, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
using qpStaticStringBase = qpStringBase< _type_, false, _encoding_, _capacity_ >;

template < uint32 _capacity_ >
using qpStaticString = qpStaticStringBase< char, _capacity_ >;
template < uint32 _capacity_ >
using qpStaticWideString = qpStaticStringBase< wchar_t, _capacity_ >;

template < typename _type_, uint32 _capacity_, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
using qpDynamicStaticStringBase = qpStringBase< _type_, true, _encoding_, _capacity_ >;
template < uint32 _capacity_ >
using qpDynamicStaticString = qpDynamicStaticStringBase< char, _capacity_ >;
template < uint32 _capacity_ >
using qpDynamicStaticWideString = qpDynamicStaticStringBase< wchar_t, _capacity_ >;


template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase() {}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( const int capacity )  requires ( _allowAlloc_ ) {
	QP_ASSERT_MSG( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( const int length, const _type_ charToInsert ) requires ( _allowAlloc_ ) {
	QP_ASSERT_MSG( length >= 0, "Length can't be less than 0." );
	Resize( length, charToInsert );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( _type_ c ) {
	Assign( c );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( const _type_ * string ) {
	Assign( string );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( const qpStringBase & other ) {
	Assign( other );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::qpStringBase( qpStringBase && rhs ) noexcept {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
	if ( rhs.IsAllocated() ) {
		m_data = rhs.m_data;
	} else {
		m_data = m_staticBuffer;
		QP_ASSERT( rhs.m_capacity == _staticBufferCapacity_ );
		qpCopyBytes( m_data, _staticBufferCapacity_ * sizeof( _type_ ), rhs.m_data, _staticBufferCapacity_ * sizeof( _type_ ) );
	}

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;

	rhs.m_length = 0;
	rhs.m_capacity = _staticBufferCapacity_;
	rhs.m_data = rhs.m_staticBuffer;

}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::~qpStringBase() {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Assign( const _type_ * string, const int length ) {
	Resize( length );
	qpCopyBytes( m_data, m_capacity * sizeof( _type_ ), string, m_length * sizeof( _type_ ) );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Assign( const _type_ c ) {
	return Assign( &c, sizeof( c ) );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Assign( const _type_ * string ) {
	return Assign( string, qpStrLen( string ) );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Assign( const qpStringBase & string ) {
	return Assign( string.m_data, string.m_length );
}

template<>
inline qpStringBase< char, true > & qpStringBase< char, true >::Format( const char * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );
	va_list args;
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

	va_list args;
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

	va_list args;
	va_start( args, format );
	int length = vswprintf( NULL, 0, format, args );
	va_end( args );

	Reserve( length + 1 );

	va_start( args, format );
	length = vswprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template<>
inline qpStringBase< wchar_t, false > & qpStringBase< wchar_t, false >::Format( const wchar_t * const format, ... ) {
	QP_ASSERT( format != NULL );
	QP_ASSERT( m_data != NULL );

	va_list args;
	va_start( args, format );
	int length = vswprintf( m_data, m_capacity, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
int qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Compare( const _type_ * string ) const {
	return qpStrCmp< _type_ >( m_data, string );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
int qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Compare( const qpStringBase & string ) const {
	return Compare( string.m_data );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Resize( const int requestedLength, const _type_ charToInsert ) {
	if ( m_length == requestedLength ) {
		return;
	}

	if constexpr ( _allowAlloc_ ) {
		Reserve( requestedLength + 1 );
		QP_ASSERT( requestedLength < m_capacity );
	} else {
		QP_ASSERT( requestedLength < _staticBufferCapacity_ );
	}

	const int clampedLength = qpMath::Min( requestedLength, ( qpVerifyStaticCast< int >( m_capacity ) - 1 ) );
	const int lengthDiff = clampedLength - m_length;

	if ( lengthDiff > 0 ) {
		qpSetMemory( m_data + ( m_length * sizeof( _type_ ) ), charToInsert, lengthDiff * sizeof( _type_ ) );
	} else if ( lengthDiff < 0 ) {
		qpSetMemory( m_data + ( ( static_cast< size_t >( m_length ) + lengthDiff ) * sizeof( _type_ ) ), 0, qpMath::Abs( lengthDiff ) * sizeof( _type_ ) );
	}

	m_length = clampedLength;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Resize( const int requestedLength ) {
	Resize( requestedLength, charTraits_t::NIL_CHAR );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Reserve( const int requestedCapacity )  requires( _allowAlloc_ ) {
	if ( m_capacity >= requestedCapacity ) {
		return;
	}

	QP_ASSERT( requestedCapacity > _staticBufferCapacity_ );

	int capacity = static_cast< int >( qpAllocationUtil::AlignSize( requestedCapacity, 8 * sizeof( _type_ ) ) );
	_type_ * newData = new _type_[ capacity ] { };
	qpCopyBytes( newData, capacity * sizeof( _type_ ), m_data, m_length * sizeof( _type_ ) );
	FreeMemory();
	m_data = newData;
	m_capacity = capacity;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::ShrinkToFit()  requires( _allowAlloc_ ) {
	if ( !IsAllocated() ) {
		return;
	}
	int fittedCapacity = qpMath::Max( ( m_length + 1 ), qpVerifyStaticCast< int >( _staticBufferCapacity_ ) );
	if ( m_capacity == fittedCapacity ) {
		return;
	}

	_type_ * fittedData = ( fittedCapacity == _staticBufferCapacity_ ) ? m_staticBuffer : new _type_[ fittedCapacity ] {};
	qpCopyBytes( fittedData, fittedCapacity * sizeof( _type_ ), m_data, ( m_length + 1 ) * sizeof(_type_));
	FreeMemory();
	m_data = fittedData;
	m_capacity = fittedCapacity;

}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Clear() {
	if ( m_data != NULL ) {
		m_data[ 0 ] = charTraits_t::NIL_CHAR;
	}
	m_length = 0;
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::ClearAndFreeMemory() {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
	Clear();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
void qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::FreeMemory() requires ( _allowAlloc_ ) {
	if ( IsAllocated() ) {
		delete[] m_data;
		m_data = m_staticBuffer;
		m_capacity = _staticBufferCapacity_;
	}
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
bool qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::IsAllocated() const {
	if constexpr ( _allowAlloc_ ) {
		return m_data != m_staticBuffer;
	} else {
		return false;
	}
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
_type_ & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::At( const int index ) {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
const _type_ & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::At( const int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
int qpStringBase<_type_, _allowAlloc_, _encoding_, _staticBufferCapacity_>::Length () const {
	if constexpr ( STRING_ENCODING == stringEncoding_t::UTF8 ) {
		return qpStrCodePoints< _type_, STRING_ENCODING >( m_data );
	} else {
		return m_length;
	};
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Find( const _type_ searchChar ) const {
	const _type_ searchStr[ 2 ] {
		searchChar,
		charTraits_t::NIL_CHAR
	};
	return Find( searchStr );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::ReverseFind( const _type_ searchChar ) const {
	const _type_ searchStr[ 2 ] {
		searchChar,
		charTraits_t::NIL_CHAR
	};
	return ReverseFind( searchStr );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Find( const _type_ * searchStr ) const {
	if ( qpStrEmpty( searchStr ) || IsEmpty() ) {
		return End();
	}
	if constexpr ( STRING_ENCODING == stringEncoding_t::UTF8 ) {
		if ( !qpStrIsCodePoint< _type_, STRING_ENCODING >( searchStr[ 0 ] ) ) {
			qpLog::Error( "Invalid UTF8 String" );
			return End();
		}
	}
	int pos = 0;
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, pos, searchDirection_t::FORWARD );
			posEnd = pos;
			searchPos = 0;
		}
	} while ( ( searchStr[ searchPos ] != charTraits_t::NIL_CHAR ) && ( pos < m_length ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::ReverseFind( const _type_ * searchStr ) const {
	if ( qpStrEmpty( searchStr ) || IsEmpty() ) {
		return End();
	}
	if constexpr ( STRING_ENCODING == stringEncoding_t::UTF8 ) {
		if ( !qpStrIsCodePoint< _type_, STRING_ENCODING >( searchStr[ 0 ] ) ) {
			qpLog::Error( "Invalid UTF8 String" );
			return End();
		}
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
	if ( !qpStrIsCodePoint< _type_, STRING_ENCODING >( m_data[ pos ] ) ) {
		pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, m_length, pos, searchDirection_t::BACKWARD );
	}
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, m_length, pos, searchDirection_t::BACKWARD );
			posEnd = pos;
			searchPos = 0;
		} 
	} while ( ( ( posEnd - pos ) != searchStrLen ) && ( pos >= 0 ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Find( const qpStringBase & searchStr ) const {
	if ( searchStr.IsEmpty() || IsEmpty() || ( searchStr.m_length > m_length ) ) {
		return End();
	}
	if constexpr ( STRING_ENCODING == stringEncoding_t::UTF8 ) {
		if ( !qpStrIsCodePoint< _type_, STRING_ENCODING >( searchStr[ 0 ] ) ) {
			qpLog::Error( "Invalid UTF8 String" );
			return End();
		}
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
			pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, pos, searchDirection_t::FORWARD );
			posEnd = pos;
			searchPos = 0;
		}
	} while ( ( searchStr[ searchPos ] != charTraits_t::NIL_CHAR ) && ( pos < m_length ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
typename qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::Iterator qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::ReverseFind( const qpStringBase & searchStr ) const {
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
	if ( !qpStrIsCodePoint< _type_, STRING_ENCODING >( m_data[ pos ] ) ) {
		pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, m_length, pos, searchDirection_t::BACKWARD );
	}
	int posEnd = pos;
	int searchPos = 0;
	do {
		if ( searchStr[ searchPos++ ] != m_data[ posEnd++ ] ) {
			pos = qpStrFindNextCodePoint< _type_, STRING_ENCODING >( m_data, m_length, pos, searchDirection_t::BACKWARD );
			posEnd = pos;
			searchPos = 0;
		}
	} while ( ( ( posEnd - pos ) != searchStr.Length() ) && ( pos >= 0 ) && ( posEnd < m_length ) && ( m_data[ posEnd ] != charTraits_t::NIL_CHAR ) );

	const bool found = ( searchPos != 0 ) && ( searchStr[ searchPos ] == charTraits_t::NIL_CHAR );
	if ( found && ( pos >= 0 ) && ( posEnd <= m_length ) ) {
		return Iterator { m_data + pos };
	}

	return End();
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator+=( const _type_ rhs ) {
	constexpr int rhsLength = 1;
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytes( m_data + m_length, m_capacity * sizeof( _type_ ), &rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator+=( const _type_ * rhs ) {
	const int rhsLength = qpStrLen( rhs );
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytes( m_data + m_length, m_capacity * sizeof( _type_ ), &rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator+=( const qpStringBase & rhs ) {
	Reserve( m_length + rhs.m_length + 1 );
	qpCopyBytes( m_data + m_length, m_capacity * sizeof( _type_ ), &rhs, rhs.m_length * sizeof( _type_ ) );
	m_length += rhs.m_length;
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator=( const _type_ rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator=( const _type_ * rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator=( const qpStringBase & rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ > & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator=( qpStringBase && rhs ) noexcept {
	if constexpr ( _allowAlloc_ ) {
		FreeMemory();
	}
	if ( rhs.IsAllocated() ) {
		m_data = rhs.m_data;
	} else {
		m_data = m_staticBuffer;
		QP_ASSERT( rhs.m_capacity == _staticBufferCapacity_ );
		qpCopyBytes( m_data, _staticBufferCapacity_ * sizeof( _type_ ), rhs.m_data, _staticBufferCapacity_ * sizeof( _type_ ) );
	}

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;

	rhs.m_length = 0;
	rhs.m_capacity = _staticBufferCapacity_;
	rhs.m_data = rhs.m_staticBuffer;

	return *this;
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
_type_ & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator[]( const int index ) {
	QP_ASSERT_MSG( index >= 0 && index < ( m_length + 1 ), "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
const _type_ & qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator[]( const int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < ( m_length + 1 ), "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
auto qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator<=>( const qpStringBase & rhs ) const {
	return Compare( rhs );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
auto qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator<=>( const _type_ * rhs ) const {
	return Compare( rhs );
}

template < typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
bool qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator==( const qpStringBase & rhs ) const {
	return ( m_length == rhs.m_length ) && ( m_data[ 0 ] == rhs.m_data[ 0 ] ) && ( Compare( rhs ) == 0 );
}

template< typename _type_, bool _allowAlloc_, stringEncoding_t _encoding_, uint32 _staticBufferCapacity_ >
bool qpStringBase< _type_, _allowAlloc_, _encoding_, _staticBufferCapacity_ >::operator==( const _type_ * rhs ) const {
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
