#pragma once
#include "qp_char_traits.h"
#include "qp/common/allocation/qp_allocation_util.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_utility.h"
#include "qp/common/math/qp_math.h"
#include <cstddef>
#include <ostream>
#include <compare>
#include <stdarg.h>
#include <type_traits>

template< typename _type_ = char >
static inline int qpStrLen( const _type_ * string ) {
	int length = 0;

	while ( *string != static_cast< _type_ >( 0 ) ) {
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
static inline bool qpStrEquals( const _type_ * a, const _type_ * b ) {
	return ( qpStrCmp( a, b ) == 0 );
}

enum class stringEncoding_t {
	DEFAULT,
	ASCII,
	UTF8,
	UTF16,
	UTF32,
	CHAR = ASCII,
#if defined( QP_PLATFORM_WINDOWS )
	WIDE_CHAR = UTF16,
#else
#error "WIDE_CHAR has not been defined for platform!"
#endif
};

template< typename _type_, stringEncoding_t _encoding_ = stringEncoding_t::DEFAULT >
class qpStringBase {
public:
	struct Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = _type_;
		using pointer = _type_ *;
		using reference = _type_ &;

		Iterator( pointer ptr ) : m_ptr( ptr ) {}

		reference operator *() const { return *m_ptr; }

		pointer operator->() { return m_ptr; }
		Iterator & operator++() {
			m_ptr++;
			return *this;
		}
		Iterator operator++( int ) {
			Iterator it = m_ptr++;
			return it;
		}

		auto operator<=>( const qpStringBase< _type_, _encoding_ >::Iterator & ) const = default;
		bool operator==( const qpStringBase< _type_, _encoding_ >::Iterator & ) const = default;

	private:
		pointer m_ptr = NULL;
	};

	qpStringBase();
	explicit qpStringBase( const int capacity );
	qpStringBase( const int length, const _type_ charToInsert );
	qpStringBase( const _type_ c );
	qpStringBase( const _type_ * string );
	qpStringBase( const qpStringBase< _type_, _encoding_ > & other );
	qpStringBase( qpStringBase< _type_, _encoding_ > && other ) noexcept;
	~qpStringBase();

	qpStringBase< _type_, _encoding_ > & Assign( const _type_ * string, const int length );
	qpStringBase< _type_, _encoding_ > & Assign( const _type_ c );
	qpStringBase< _type_, _encoding_ > & Assign( const _type_ * string );
	qpStringBase< _type_, _encoding_ > & Assign( const qpStringBase< _type_, _encoding_ > & string );

	qpStringBase< _type_, _encoding_ > & Format( const _type_ * const format, ... );

	int Compare( const _type_ * string ) const;
	int Compare( const qpStringBase< _type_, _encoding_ > & string ) const;

	void Resize( const int newLength, const _type_ charToInsert );
	void Resize( const int newLength );
	void Reserve( const int requestedCapacity );
	void ShrinkToFit();

	void Clear();

	_type_ & At( int index );
	const _type_ & At( int index ) const;

	int LengthBytes() const { return m_length * sizeof( _type_ ); }
	int Length() const { 
		if constexpr ( _encoding_ == stringEncoding_t::UTF8 ) { 
			return qpStrUTF8CodePoints( m_data ); 
		} else {
			return m_length; 
		}; 
	}
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0; }

	stringEncoding_t GetEncoding() const { return _encoding_; }

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

	qpStringBase< _type_, _encoding_ > & operator+=( const _type_ rhs );
	qpStringBase< _type_, _encoding_ > & operator+=( const _type_ * rhs );
	qpStringBase< _type_, _encoding_ > & operator+=( const qpStringBase< _type_, _encoding_ > & rhs );
	qpStringBase< _type_, _encoding_ > & operator=( const _type_ rhs );
	qpStringBase< _type_, _encoding_ > & operator=( const _type_ * rhs );
	qpStringBase< _type_, _encoding_ > & operator=( const qpStringBase< _type_, _encoding_ > & rhs );
	qpStringBase< _type_, _encoding_ > & operator=( qpStringBase< _type_, _encoding_ > && rhs ) noexcept;
	_type_ & operator[]( int index );
	const _type_ & operator[]( int index ) const;

	auto operator<=>( const qpStringBase< _type_, _encoding_ > & rhs ) const;
	auto operator<=>( const _type_ * rhs ) const;
	bool operator==( const qpStringBase< _type_, _encoding_ > & rhs ) const;

	friend qpStringBase< _type_, _encoding_ > operator+( _type_ lhs, const qpStringBase< _type_, _encoding_ > & rhs ) {
		qpStringBase< _type_, _encoding_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_, _encoding_ > operator+( const qpStringBase< _type_, _encoding_ > & lhs, _type_ rhs ) {
		qpStringBase< _type_, _encoding_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_, _encoding_ > operator+( const _type_ * lhs, const qpStringBase< _type_, _encoding_ > & rhs ) {
		qpStringBase< _type_, _encoding_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_, _encoding_ > operator+( const qpStringBase< _type_, _encoding_ > & lhs, const _type_ * rhs ) {
		qpStringBase< _type_, _encoding_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_, _encoding_ > operator+( const qpStringBase< _type_, _encoding_ > & lhs, const qpStringBase< _type_, _encoding_ > & rhs ) {
		// makes sure there is only one allocation by allocating space for both at once
		qpStringBase< _type_, _encoding_ > result( lhs.m_length + rhs.m_length );
		result = lhs;
		result += rhs;
		return result;
	}

	friend std::basic_ostream< _type_ > & operator<<( std::basic_ostream< _type_ > & out, const qpStringBase< _type_, _encoding_ > & string ) {
		return out << string.m_data;
	}

private:
	int m_capacity = 0;
	int m_length = 0;
	_type_ * m_data = NULL;
};

using qpString = qpStringBase< char, stringEncoding_t::ASCII >;
using qpU8String = qpStringBase< char8_t, stringEncoding_t::UTF8 >;
using qpWideString = qpStringBase< wchar_t, stringEncoding_t::WIDE_CHAR >;

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase() {
	m_capacity = 16;
	m_length = 0;
	m_data = new _type_[ m_capacity ] { };
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( const int capacity ) {
	QP_ASSERT_MSG( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

template< typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( const int length, const _type_ charToInsert ) {
	QP_ASSERT_MSG( length >= 0, "Capacity can't be less than 0." );
	Resize( length, charToInsert );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( _type_ c ) {
	Assign( c );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( const _type_ * string ) {
	Assign( string );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( const qpStringBase< _type_, _encoding_ > & other ) {
	m_length = other.m_length;
	m_capacity = other.m_length + 1;
	m_data = new _type_[ m_capacity ] { };
	qpCopyBytesUnchecked( m_data, other.m_data, other.m_length * sizeof( _type_ ) );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::qpStringBase( qpStringBase< _type_, _encoding_ > && other ) noexcept {
	m_length = other.m_length;
	m_capacity = other.m_capacity;
	m_data = other.m_data;

	other.m_data = NULL;
	other.m_length = 0;
	other.m_capacity = 0;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ >::~qpStringBase() {
	delete[] m_data;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::Assign( const _type_ * string, const int length ) {
	Reserve( length + 1 );
	qpCopyBytesUnchecked( m_data, string, length * sizeof( _type_ ) );
	m_length = length;
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::Assign( const _type_ c ) {
	return Assign( &c, sizeof( c ) );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::Assign( const _type_ * string ) {
	return Assign( string, qpStrLen( string ) );
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::Assign( const qpStringBase< _type_, _encoding_ > & string ) {
	return Assign( string.m_data, string.m_length );
}

template<>
inline qpStringBase< char, stringEncoding_t::ASCII > & qpStringBase< char, stringEncoding_t::ASCII >::Format( const char * const format, ... ) {
	QP_ASSERT( format != NULL );

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
inline qpStringBase< wchar_t, stringEncoding_t::WIDE_CHAR > & qpStringBase< wchar_t, stringEncoding_t::WIDE_CHAR >::Format( const wchar_t * const format, ... ) {
	QP_ASSERT( format != NULL );

	va_list args = NULL;
	va_start( args, format );
	int length = _vsnwprintf( NULL, 0, format, args );
	va_end( args );

	Reserve( length + 1 );

	va_start( args, format );
	length = _vsnwprintf( m_data, length, format, args );
	va_end( args );

	QP_ASSERT_MSG( length >= 0, "Failed to format string." );

	m_length = length;

	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
int qpStringBase< _type_, _encoding_ >::Compare( const _type_ * string ) const {
	return qpStrCmp< _type_ >( m_data, string );
}

template < typename _type_, stringEncoding_t _encoding_ >
int qpStringBase< _type_, _encoding_ >::Compare( const qpStringBase< _type_, _encoding_ > & string ) const {
	return Compare( string.m_data );
}

template < typename _type_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _encoding_ >::Resize( const int newLength, const _type_ charToInsert ) {
	if ( m_length == newLength ) {
		return;
	}

	Reserve( newLength + 1 );

	int lengthDiff = newLength - m_length;

	if ( lengthDiff > 0 ) {
		qpSetMemory( m_data + ( m_length * sizeof( _type_ ) ), charToInsert, lengthDiff * sizeof( _type_ ) );
	} else if ( lengthDiff < 0 ) {
		qpSetMemory( m_data + ( ( static_cast< size_t >( m_length ) + lengthDiff ) * sizeof( _type_ ) ), 0, qpMath::Abs( lengthDiff ) * sizeof( _type_ ) );
	}

	m_length = newLength;
}

template < typename _type_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _encoding_ >::Resize( const int newLength ) {
	Resize( newLength, static_cast< _type_ >( 0 ) );
}

template < typename _type_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _encoding_ >::Reserve( const int requestedCapacity ) {
	if ( m_capacity >= requestedCapacity ) {
		return;
	}

	int capacity = static_cast< int >( qpAllocationUtil::AlignSize( requestedCapacity, 16 * sizeof( _type_ ) ) );

	_type_ * newData = new _type_[ capacity ] { };
	qpCopyBytesUnchecked( newData, m_data, m_capacity * sizeof( _type_ ) );
	delete m_data;
	m_data = newData;

	m_capacity = capacity;
}

template < typename _type_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _encoding_ >::ShrinkToFit() {
	int fittedCapacity = m_length + 1;
	if ( m_capacity == ( fittedCapacity ) ) {
		return;
	}

	_type_ * fittedData = new _type_[ fittedCapacity ] { };
	qpCopyBytesUnchecked( fittedData, m_data, fittedCapacity * sizeof( _type_ ) );
	delete m_data;
	m_data = fittedData;

	m_capacity = fittedCapacity;
}

template < typename _type_, stringEncoding_t _encoding_ >
void qpStringBase< _type_, _encoding_ >::Clear() {
	if ( m_data != NULL ) {
		m_data[ 0 ] = static_cast< _type_ >( 0 );
	}
	m_length = 0;
}

template < typename _type_, stringEncoding_t _encoding_ >
_type_ & qpStringBase< _type_, _encoding_ >::At( const int index ) {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, stringEncoding_t _encoding_ >
const _type_ & qpStringBase< _type_, _encoding_ >::At( const int index ) const {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator+=( const _type_ rhs ) {
	constexpr int rhsLength = 1;
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytesUnchecked( m_data + m_length, &rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator+=( const _type_ * rhs ) {
	const int rhsLength = qpStrLen( rhs );
	Reserve( m_length + rhsLength + 1 );
	qpCopyBytesUnchecked( m_data + m_length, rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator+=( const qpStringBase< _type_, _encoding_ > & rhs ) {
	Reserve( m_length + rhs.m_length + 1 );
	qpCopyBytesUnchecked( m_data + m_length, rhs.m_data, rhs.m_length * sizeof( _type_ ) );
	m_length += rhs.m_length;
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator=( const _type_ rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator=( const _type_ * rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator=( const qpStringBase< _type_, _encoding_ > & rhs ) {
	Assign( rhs );
	return *this;
}

template< typename _type_, stringEncoding_t _encoding_ >
qpStringBase< _type_, _encoding_ > & qpStringBase< _type_, _encoding_ >::operator=( qpStringBase< _type_, _encoding_ > && rhs ) noexcept {
	delete m_data;

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;
	m_data = rhs.m_data;

	rhs.m_length = 0;
	rhs.m_capacity = 0;
	rhs.m_data = NULL;

	return *this;
}

template < typename _type_, stringEncoding_t _encoding_ >
_type_ & qpStringBase< _type_, _encoding_ >::operator[]( const int index ) {
	return At( index );
}

template < typename _type_, stringEncoding_t _encoding_ >
const _type_ & qpStringBase< _type_, _encoding_ >::operator[]( const int index ) const {
	return At( index );
}

template< typename _type_, stringEncoding_t _encoding_ >
auto qpStringBase< _type_, _encoding_ >::operator<=>( const qpStringBase< _type_, _encoding_ > & rhs ) const {
	return Compare( rhs );
}

template< typename _type_, stringEncoding_t _encoding_ >
auto qpStringBase< _type_, _encoding_ >::operator<=>( const _type_ * rhs ) const {
	return Compare( rhs );
}

template< typename _type_, stringEncoding_t _encoding_ >
bool qpStringBase< _type_, _encoding_ >::operator==( const qpStringBase< _type_, _encoding_ > & rhs ) const {
	return ( m_length == rhs.m_length ) && ( m_data[ 0 ] == rhs.m_data[ 0 ] ) && Compare( rhs );
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
