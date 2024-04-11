#pragma once
#include "qp/common/allocation/qp_allocation_util.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_utility.h"
#include "qp/common/math/qp_math.h"
#include <cstddef>
#include <ostream>
#include <compare>
#include <stdarg.h>
#include <type_traits>

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/platform/windows/qp_windows.h"
#endif

template< typename _type_ = char >
static inline int qpStrLen( const _type_ * string ) {
	int length = 0;

	while ( *string != static_cast< _type_ >( 0 ) ) {
		++length;
		++string;
	}
	return length;
}

template< typename _type_ = char >
static inline bool qpStrCmp( const _type_ * a, const _type_ * b ) {
	using unsignedType = std::make_unsigned_t< _type_ >;
	const unsignedType * ua = reinterpret_cast< const unsignedType * > ( a );
	const unsignedType * ub = reinterpret_cast< const unsignedType * > ( b );

	while ( *ua && ( *ua == *ub ) ) {
		++ua;
		++ub;
	}
	return ( *ua > *ub ) - ( *ub > *ua );
}

template < typename _type_ = char >
static inline bool qpStrEquals( const _type_ * a, const _type_ * b ) {
	return ( qpStrCmp( a, b ) == 0 );
}

template< typename _type_ >
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

		auto operator<=>( const qpStringBase< _type_ >::Iterator & ) const = default;
		bool operator==( const qpStringBase< _type_ >::Iterator & ) const = default;

	private:
		pointer m_ptr = NULL;
	};

	qpStringBase();
	explicit qpStringBase( int capacity );
	qpStringBase( const _type_ c );
	qpStringBase( const _type_ * string );
	qpStringBase( const qpStringBase< _type_ > & other );
	qpStringBase( qpStringBase< _type_ > && other ) noexcept;
	~qpStringBase();

	qpStringBase< _type_ > & Assign( const _type_ * string, const int length );
	qpStringBase< _type_ > & Assign( const _type_ c );
	qpStringBase< _type_ > & Assign( const _type_ * string );
	qpStringBase< _type_ > & Assign( const qpStringBase< _type_ > & string );

	qpStringBase< _type_ > & Format( const _type_ * const format, ... );

	int Compare( const _type_ * string ) const;
	int Compare( const qpStringBase< _type_ > & string ) const;

	void Resize( const int newLength, const _type_ charToInsert );
	void Resize( const int newLength );
	void Reserve( const int requestedCapacity );
	void ShrinkToFit();

	void Clear();

	_type_ & At( int index );
	const _type_ & At( int index ) const;

	int Length() const { return m_length; }
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0; }

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

	qpStringBase< _type_ > & operator+=( const _type_ rhs );
	qpStringBase< _type_ > & operator+=( const _type_ * rhs );
	qpStringBase< _type_ > & operator+=( const qpStringBase< _type_ > & rhs );
	qpStringBase< _type_ > & operator=( const _type_ rhs );
	qpStringBase< _type_ > & operator=( const _type_ * rhs );
	qpStringBase< _type_ > & operator=( const qpStringBase< _type_ > & rhs );
	qpStringBase< _type_ > & operator=( qpStringBase< _type_ > && rhs ) noexcept;
	_type_ & operator[]( int index );
	const _type_ & operator[]( int index ) const;

	auto operator<=>( const qpStringBase< _type_ > & rhs ) const;
	auto operator<=>( const _type_ * rhs ) const;
	bool operator==( const qpStringBase< _type_ > & rhs ) const;

	friend qpStringBase< _type_ > operator+( _type_ lhs, const qpStringBase< _type_ > & rhs ) {
		qpStringBase< _type_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_ > operator+( const qpStringBase< _type_ > & lhs, _type_ rhs ) {
		qpStringBase< _type_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_ > operator+( const _type_ * lhs, const qpStringBase< _type_ > & rhs ) {
		qpStringBase< _type_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_ > operator+( const qpStringBase< _type_ > & lhs, const _type_ * rhs ) {
		qpStringBase< _type_ > result( lhs );
		result += rhs;
		return result;
	}

	friend qpStringBase< _type_ > operator+( const qpStringBase< _type_ > & lhs, const qpStringBase< _type_ > & rhs ) {
		// makes sure there is only one allocation by allocating space for both at once
		qpStringBase< _type_ > result( lhs.m_length + rhs.m_length );
		result = lhs;
		result += rhs;
		return result;
	}

	friend std::basic_ostream< _type_ > & operator<<( std::basic_ostream< _type_ > & out, const qpStringBase< _type_ > & string ) {
		return out << string.m_data;
	}

private:
	int m_capacity = 0;
	int m_length = 0;
	_type_ * m_data = NULL;
};

using qpString = qpStringBase< char >;
using qpWideString = qpStringBase< wchar_t >;

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase() {
	m_capacity = 16;
	m_length = 0;
	m_data = new _type_[ m_capacity ] { };
}

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase( const int capacity ) {
	QP_ASSERT_MSG( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase( _type_ c ) {
	Assign( c );
}

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase( const _type_ * string ) {
	Assign( string );
}

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase( const qpStringBase< _type_ > & other ) {
	m_length = other.m_length;
	m_capacity = other.m_length + 1;
	m_data = new _type_[ m_capacity ] { };
	memcpy( m_data, other.m_data, other.m_length * sizeof( _type_ ) );
}

template < typename _type_ >
qpStringBase< _type_ >::qpStringBase( qpStringBase< _type_ > && other ) noexcept {
	m_length = other.m_length;
	m_capacity = other.m_capacity;
	m_data = other.m_data;

	other.m_data = NULL;
	other.m_length = 0;
	other.m_capacity = 0;
}

template < typename _type_ >
qpStringBase< _type_ >::~qpStringBase() {
	delete[] m_data;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::Assign( const _type_ * string, const int length ) {
	Reserve( length + 1 );
	memcpy( m_data, string, length * sizeof( _type_ ) );
	m_length = length;
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::Assign( const _type_ c ) {
	return Assign( &c, sizeof( c ) );
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::Assign( const _type_ * string ) {
	return Assign( string, qpStrLen( string ) );
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::Assign( const qpStringBase< _type_ > & string ) {
	return Assign( string.m_data, string.m_length );
}

template<>
inline qpStringBase< char > & qpStringBase< char >::Format( const char * const format, ... ) {
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
inline qpStringBase< wchar_t > & qpStringBase< wchar_t >::Format( const wchar_t * const format, ... ) {
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

template < typename _type_ >
int qpStringBase< _type_ >::Compare( const _type_ * string ) const {
	return qpStrCmp< _type_ >( m_data, string );
}

template < typename _type_ >
int qpStringBase< _type_ >::Compare( const qpStringBase< _type_ > & string ) const {
	return Compare( string.m_data );
}

template < typename _type_ >
void qpStringBase< _type_ >::Resize( const int newLength, const _type_ charToInsert ) {
	if ( m_length == newLength ) {
		return;
	}

	Reserve( newLength + 1 );

	int lengthDiff = newLength - m_length;

	if ( lengthDiff > 0 ) {
		memset( m_data + ( m_length * sizeof( _type_ ) ), charToInsert, lengthDiff * sizeof( _type_ ) );
	} else if ( lengthDiff < 0 ) {
		memset( m_data + ( ( static_cast< size_t >( m_length ) + lengthDiff ) * sizeof( _type_ ) ), 0, qpMath::Abs( lengthDiff ) * sizeof( _type_ ) );
	}

	m_length = newLength;
}

template < typename _type_ >
void qpStringBase< _type_ >::Resize( const int newLength ) {
	Resize( newLength, static_cast< _type_ >( 0 ) );
}

template < typename _type_ >
void qpStringBase< _type_ >::Reserve( const int requestedCapacity ) {
	if ( m_capacity >= requestedCapacity ) {
		return;
	}

	int capacity = static_cast< int >( qpAllocationUtil::AlignSize( requestedCapacity, 16 * sizeof( _type_ ) ) );

	_type_ * newData = new _type_[ capacity ] { };
	memcpy( newData, m_data, m_capacity * sizeof( _type_ ) );
	delete m_data;
	m_data = newData;

	m_capacity = capacity;
}

template < typename _type_ >
void qpStringBase< _type_ >::ShrinkToFit() {
	int fittedCapacity = m_length + 1;
	if ( m_capacity == ( fittedCapacity ) ) {
		return;
	}

	_type_ * fittedData = new _type_[ fittedCapacity ] { };
	memcpy( fittedData, m_data, fittedCapacity * sizeof( _type_ ) );
	delete m_data;
	m_data = fittedData;

	m_capacity = fittedCapacity;
}

template < typename _type_ >
void qpStringBase< _type_ >::Clear() {
	if ( m_data != NULL ) {
		m_data[ 0 ] = static_cast< _type_ >( 0 );
	}
	m_length = 0;
}

template < typename _type_ >
_type_ & qpStringBase< _type_ >::At( const int index ) {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_ >
const _type_ & qpStringBase< _type_ >::At( const int index ) const {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator+=( const _type_ rhs ) {
	constexpr int rhsLength = 1;
	Reserve( m_length + rhsLength + 1 );
	memcpy( m_data + m_length, &rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator+=( const _type_ * rhs ) {
	const int rhsLength = qpStrLen( rhs );
	Reserve( m_length + rhsLength + 1 );
	memcpy( m_data + m_length, rhs, rhsLength * sizeof( _type_ ) );
	m_length += rhsLength;
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator+=( const qpStringBase< _type_ > & rhs ) {
	Reserve( m_length + rhs.m_length + 1 );
	memcpy( m_data + m_length, rhs.m_data, rhs.m_length * sizeof( _type_ ) );
	m_length += rhs.m_length;
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator=( const _type_ rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator=( const _type_ * rhs ) {
	Assign( rhs );
	return *this;
}

template < typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator=( const qpStringBase< _type_ > & rhs ) {
	Assign( rhs );
	return *this;
}

template< typename _type_ >
qpStringBase< _type_ > & qpStringBase< _type_ >::operator=( qpStringBase< _type_ > && rhs ) noexcept {
	delete m_data;

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;
	m_data = rhs.m_data;

	rhs.m_length = 0;
	rhs.m_capacity = 0;
	rhs.m_data = NULL;

	return *this;
}

template < typename _type_ >
_type_ & qpStringBase< _type_ >::operator[]( const int index ) {
	return At( index );
}

template < typename _type_ >
const _type_ & qpStringBase< _type_ >::operator[]( const int index ) const {
	return At( index );
}

template< typename _type_ >
auto qpStringBase< _type_ >::operator<=>( const qpStringBase< _type_ > & rhs ) const {
	return Compare( rhs );
}

template< typename _type_ >
auto qpStringBase< _type_ >::operator<=>( const _type_ * rhs ) const {
	return Compare( rhs );
}

template< typename _type_ >
bool qpStringBase< _type_ >::operator==( const qpStringBase< _type_ > & rhs ) const {
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
static inline qpWideString qpFormat( const wchar_t * const format, _args_&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpWideString formatted( 0 );
	formatted.Format( format, qpForward< _args_ >( args )... );
	return formatted;
}

static inline qpWideString qpUTF8ToWide( const qpString & string );
static inline qpString qpWideToUTF8( const qpWideString & string );

#if defined( QP_PLATFORM_WINDOWS )

static inline qpWideString qpUTF8ToWide( const qpString & string ) {
	int length = ::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0 );

	if ( length == 0 ) {
		return qpWideString {};
	}

	qpWideString convertedString( length + 1 );
	::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length );
	return convertedString;
}

static inline qpString qpWideToUTF8( const qpWideString & string ) {
	int length = ::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0, NULL, NULL );

	if ( length == 0 ) {
		return qpString {};
	}

	qpString convertedString( string.Length() + 1 );
	::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length, NULL, NULL );
	return convertedString;
}

#endif