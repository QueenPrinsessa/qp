#pragma once
#include "qp/tools/allocation/qp_allocation_util.h"
#include "qp/common/debug/qp_debug.h"
#include "qp/common/utilities/qp_utility.h"
#include "qp/tools/math/qp_math.h"
#include <cstddef>
#include <ostream>
#include <compare>
#include <type_traits>

#ifdef QP_PLATFORM_WINDOWS
#include "qp/common/platform/windows/qp_windows.h"
#endif

template< typename T = char >
static inline int qpStrLen( const T * string ) {
	int length = 0;

	while ( *string != static_cast< T >( 0 ) ) {
		++length;
		++string;
	}
	return length;
}

template< typename T = char, typename UNSIGNED_T = std::make_unsigned_t< T > >
static inline bool qpStrCmp( const T * a, const T * b ) {
	const UNSIGNED_T * ua = reinterpret_cast< const UNSIGNED_T * > ( a );
	const UNSIGNED_T * ub = reinterpret_cast< const UNSIGNED_T * > ( b );

	while ( *ua && ( *ua == *ub ) ) {
		++ua;
		++ub;
	}
	return ( *ua > *ub ) - ( *ub > *ua );
}

template < typename T = char >
static inline bool qpStrEquals( const T * a, const T * b ) {
	return ( qpStrCmp( a, b ) == 0 );
}

template< typename T >
class qpTString
{
public:
	struct Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;

		Iterator( pointer ptr ) : m_ptr( ptr ) { }

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

		auto operator<=>( const qpTString< T >::Iterator & ) const = default;
		bool operator==( const qpTString< T >::Iterator & ) const = default;

	private:
		pointer m_ptr = NULL;
	};

	qpTString();
	explicit qpTString( int capacity );
	qpTString( const T c );
	qpTString( const T * string );
	qpTString( const qpTString< T > & other );
	qpTString( qpTString< T > && other ) noexcept;
	~qpTString();

	qpTString< T > & Assign( const T * string, const int length );
	qpTString< T > & Assign( const T c );
	qpTString< T > & Assign( const T * string );
	qpTString< T > & Assign( const qpTString< T > & string );

	qpTString< T > & Format( const T * const format, ... );

	int Compare( const T * string ) const;
	int Compare( const qpTString< T > & string ) const;

	void Resize( const int newLength, const T charToInsert );
	void Resize( const int newLength );
	void Reserve( const int requestedCapacity );
	void ShrinkToFit();

	void Clear();

	T & At( int index );
	const T & At( int index ) const;

	int Length() const { return m_length; }
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0; }

	T * Data() const { return m_data; }
	const T * c_str() const { return m_data; }

	Iterator Begin() { return Iterator( &m_data[ 0 ] ); }
	Iterator End() { return Iterator( &m_data[ m_length ] ); }
	Iterator Begin() const { return Iterator( &m_data[ 0 ] ); }
	Iterator End() const { return Iterator( &m_data[ m_length ] ); }
	Iterator begin() { return Begin(); }
	Iterator end() { return End(); }
	Iterator begin() const { return Begin(); }
	Iterator end() const { return End(); }

	qpTString< T > & operator+=( const T rhs );
	qpTString< T > & operator+=( const T * rhs );
	qpTString< T > & operator+=( const qpTString< T > & rhs );
	qpTString< T > & operator=( const T rhs );
	qpTString< T > & operator=( const T * rhs );
	qpTString< T > & operator=( const qpTString< T > & rhs );
	qpTString< T > & operator=( qpTString< T > && rhs ) noexcept;
	T & operator[]( int index );
	const T & operator[]( int index ) const;

	auto operator<=>( const qpTString< T > & rhs ) const;
	auto operator<=>( const T * rhs ) const;
	bool operator==( const qpTString< T > & rhs ) const;

	friend qpTString< T > operator+( T lhs, const qpTString< T > & rhs ) {
		qpTString< T > result( lhs );
		result += rhs;
		return result;
	}

	friend qpTString< T > operator+( const qpTString< T > & lhs, T rhs ) {
		qpTString< T > result( lhs );
		result += rhs;
		return result;
	}

	friend qpTString< T > operator+( const T * lhs, const qpTString< T > & rhs ) {
		qpTString< T > result( lhs );
		result += rhs;
		return result;
	}

	friend qpTString< T > operator+( const qpTString< T > & lhs, const T * rhs ) {
		qpTString< T > result( lhs );
		result += rhs;
		return result;
	}

	friend qpTString< T > operator+( const qpTString< T > & lhs, const qpTString< T > & rhs ) {
		// makes sure there is only one allocation by allocating space for both at once
		qpTString< T > result( lhs.m_length + rhs.m_length );
		result = lhs;
		result += rhs;
		return result;
	}

	friend std::basic_ostream< T > & operator<<( std::basic_ostream< T > & out, const qpTString< T > & string ) {
		return out << string.m_data;
	}

private:
	int m_capacity = 0;
	int m_length = 0;
	T * m_data = NULL;
};

using qpString = qpTString< char >;
using qpWString = qpTString< wchar_t >;

template < typename T >
qpTString< T >::qpTString() {
	m_capacity = 16;
	m_length = 0;
	m_data = new T[ m_capacity ] { };
}

template < typename T >
qpTString< T >::qpTString( const int capacity ) {
	QP_ASSERT_MSG( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

template < typename T >
qpTString< T >::qpTString( T c ) {
	Assign( c );
}

template < typename T >
qpTString< T >::qpTString( const T * string ) {
	Assign( string );
}

template < typename T >
qpTString< T >::qpTString( const qpTString< T > & other ) {
	m_length = other.m_length;
	m_capacity = other.m_length + 1;
	m_data = new T[ m_capacity ] { };
	memcpy( m_data, other.m_data, other.m_length * sizeof( T ) );
}

template < typename T >
qpTString< T >::qpTString( qpTString< T > && other ) noexcept {
	m_length = other.m_length;
	m_capacity = other.m_capacity;
	m_data = other.m_data;

	other.m_data = NULL;
	other.m_length = 0;
	other.m_capacity = 0;
}

template < typename T >
qpTString< T >::~qpTString() {
	delete[] m_data;
}

template < typename T >
qpTString< T > & qpTString< T >::Assign( const T * string, const int length ) {
	Reserve( length + 1 );
	memcpy( m_data, string, length * sizeof( T ) );
	m_length = length;
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::Assign( const T c ) {
	return Assign( &c, sizeof( c ) );
}

template < typename T >
qpTString< T > & qpTString< T >::Assign( const T * string ) {
	return Assign( string, qpStrLen( string ) );
}

template < typename T >
qpTString< T > & qpTString< T >::Assign( const qpTString< T > & string ) {
	return Assign( string.m_data, string.m_length );
}

template<>
inline qpTString< char > & qpTString< char >::Format( const char * const format, ... ) {
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
inline qpTString< wchar_t > & qpTString< wchar_t >::Format( const wchar_t * const format, ... ) {
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

template < typename T >
int qpTString< T >::Compare( const T * string ) const {
	return qpStrCmp< T >( m_data, string );
}

template < typename T >
int qpTString< T >::Compare( const qpTString< T > & string ) const {
	return Compare( string.m_data );
}

template < typename T >
void qpTString< T >::Resize( const int newLength, const T charToInsert ) {
	if ( m_length == newLength ) {
		return;
	}

	Reserve( newLength + 1 );

	int lengthDiff = newLength - m_length;

	if ( lengthDiff > 0 ) {
		memset( m_data + ( m_length * sizeof( T ) ), charToInsert, lengthDiff * sizeof( T ) );
	} else if ( lengthDiff < 0 ) {
		memset( m_data + ( ( static_cast< size_t >( m_length ) + lengthDiff ) * sizeof( T ) ), 0, qpMath::Abs( lengthDiff ) * sizeof( T ) );
	}

	m_length = newLength;
}

template < typename T >
void qpTString< T >::Resize( const int newLength ) {
	Resize( newLength, static_cast< T >( 0 ) );
}

template < typename T >
void qpTString< T >::Reserve( const int requestedCapacity ) {
	if ( m_capacity >= requestedCapacity ) {
		return;
	}

	int capacity = static_cast< int >( qpAllocationUtil::AlignSize( requestedCapacity, 16 * sizeof( T ) ) );

	T * newData = new T[ capacity ] { };
	memcpy( newData, m_data, m_capacity * sizeof( T ) );
	delete m_data;
	m_data = newData;

	m_capacity = capacity;
}

template < typename T >
void qpTString< T >::ShrinkToFit() {
	int fittedCapacity = m_length + 1;
	if ( m_capacity == ( fittedCapacity ) ) {
		return;
	}

	T * fittedData = new T[ fittedCapacity ] { };
	memcpy( fittedData, m_data, fittedCapacity * sizeof( T ) );
	delete m_data;
	m_data = fittedData;

	m_capacity = fittedCapacity;
}

template < typename T >
void qpTString< T >::Clear() {
	if ( m_data != NULL ) {
		m_data[ 0 ] = static_cast< T >( 0 );
	}
	m_length = 0;
}

template < typename T >
T & qpTString< T >::At( const int index ) {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename T >
const T & qpTString< T >::At( const int index ) const {
	QP_ASSERT_MSG( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

template < typename T >
qpTString< T > & qpTString< T >::operator+=( const T rhs ) {
	constexpr int rhsLength = 1;
	Reserve( m_length + rhsLength + 1 );
	memcpy( m_data + m_length, &rhs, rhsLength * sizeof( T ) );
	m_length += rhsLength;
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::operator+=( const T * rhs ) {
	const int rhsLength = qpStrLen( rhs );
	Reserve( m_length + rhsLength + 1 );
	memcpy( m_data + m_length, rhs, rhsLength * sizeof( T ) );
	m_length += rhsLength;
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::operator+=( const qpTString< T > & rhs ) {
	Reserve( m_length + rhs.m_length + 1 );
	memcpy( m_data + m_length, rhs.m_data, rhs.m_length * sizeof( T ) );
	m_length += rhs.m_length;
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::operator=( const T rhs ) {
	Assign( rhs );
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::operator=( const T * rhs ) {
	Assign( rhs );
	return *this;
}

template < typename T >
qpTString< T > & qpTString< T >::operator=( const qpTString< T > & rhs ) {
	Assign( rhs );
	return *this;
}

template< typename T >
qpTString< T > & qpTString< T >::operator=( qpTString< T > && rhs ) noexcept {
	delete m_data;

	m_length = rhs.m_length;
	m_capacity = rhs.m_capacity;
	m_data = rhs.m_data;

	rhs.m_length = 0;
	rhs.m_capacity = 0;
	rhs.m_data = NULL;

	return *this;
}

template < typename T >
T & qpTString< T >::operator[]( const int index ) {
	return At( index );
}

template < typename T >
const T & qpTString< T >::operator[]( const int index ) const {
	return At( index );
}

template< typename T >
auto qpTString< T >::operator<=>( const qpTString< T > & rhs ) const {
	return Compare( rhs );
}

template< typename T >
auto qpTString< T >::operator<=>( const T * rhs ) const {
	return Compare( rhs );
}

template< typename T >
bool qpTString< T >::operator==( const qpTString< T > & rhs ) const {
	return ( m_length == rhs.m_length ) && ( m_data[ 0 ] == rhs.m_data[ 0 ] ) && Compare( rhs );
}

template < typename... ARGS >
static inline qpString qpFormat( const char * const format, ARGS&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpString formatted( 0 ); 
	formatted.Format( format, qpForward< ARGS >( args )... );
	return formatted;
}

template < typename... ARGS >
static inline qpWString qpFormat( const wchar_t * const format, ARGS&&... args ) {
	// prevent allocation and let Format allocate the correct size directly.
	qpWString formatted( 0 );
	formatted.Format( format, qpForward< ARGS >( args )... );
	return formatted;
}

static inline qpWString qpUTF8ToWide( const qpString & string );
static inline qpString qpWideToUTF8( const qpWString & string );

#ifdef QP_PLATFORM_WINDOWS

static inline qpWString qpUTF8ToWide( const qpString & string ) {
	int length = ::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0 );

	if ( length == 0 ) {
		return qpWString {};
	}

	qpWString convertedString( length + 1 );
	::MultiByteToWideChar( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length );
	return convertedString;
}

static inline qpString qpWideToUTF8( const qpWString & string ) {
	int length = ::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), NULL, 0, NULL, NULL );

	if ( length == 0 ) {
		return qpString {};
	}

	qpString convertedString( string.Length() + 1 );
	::WideCharToMultiByte( CP_UTF8, 0, string.Data(), string.Length(), convertedString.Data(), length, NULL, NULL );
	return convertedString;
}

#endif