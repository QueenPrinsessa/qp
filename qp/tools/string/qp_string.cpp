#include "qp_string.h"
#include "qp/tools/debug/qp_debug.h"
#include "qp/tools/math/qp_math.h"
#include <cstring>

qpString::qpString() {
	m_capacity = 1;
	m_length = 0;
	m_data = new char[ m_capacity ] { '\0' };
}

qpString::qpString( int capacity ) {
	qpAssertMsg( capacity >= 0, "Capacity can't be less than 0." );
	Reserve( capacity );
}

qpString::qpString( char c ) {
	Assign( c );
}

qpString::qpString( const char * string ) {
	Assign( string );
}

qpString::qpString( const qpString & other ) {
	m_length = other.m_length;
	m_capacity = other.m_length + 1;
	m_data = new char[ m_capacity ];
	memset( m_data, 0, m_capacity );
	memcpy_s( m_data, m_capacity, other.m_data, other.m_length );
}

qpString::qpString( qpString && other ) noexcept {
	m_length = other.m_length;
	m_capacity = other.m_capacity;
	m_data = other.m_data;

	other.m_data = NULL;
	other.m_length = 0;
	other.m_capacity = 0;
}

qpString::~qpString() {
	delete m_data;
}

qpString & qpString::Assign( const char * string, int length ) {
	Reserve( length + 1 );
	m_length = length;
	strncpy_s( m_data, m_capacity, string, static_cast < rsize_t >( m_length ) );
	m_data[ m_length ] = '\0';
	return *this;
}

qpString & qpString::Assign ( char c ) {
	return Assign( &c, sizeof(c) );
}

qpString & qpString::Assign( const char * string ) {
	return Assign( string, static_cast< int > ( strlen( string ) ) );
}

qpString & qpString::Assign( const qpString & string ) {
	return Assign( string.m_data, string.m_length );
}

int qpString::Compare( const char * string ) const {
	return strcmp( m_data, string );
}

int qpString::Compare( const qpString & string ) const {
	return Compare( string.m_data );
}

void qpString::Resize( int newLength, char charToInsert ) {
	if ( m_length == newLength ) {
		return;
	}

	Reserve( newLength + 1 );

	int lengthDiff = newLength - m_length;

	if ( lengthDiff > 0 ) {
		memset( m_data + m_length, charToInsert, lengthDiff );
	} else if ( lengthDiff < 0 ) {
		memset( m_data + m_length + lengthDiff, 0, qpMath::Abs( lengthDiff ) );
	}

	m_length = newLength;
}

void qpString::Resize( int newLength ) {
	Resize( newLength, '\0' );
}

void qpString::Reserve( int newCapacity ) {
	if ( m_capacity >= newCapacity ) {
		return;
	}

	char * newData = new char[ newCapacity ];
	memset( newData, 0, newCapacity );
	memcpy_s( newData, newCapacity, m_data, m_capacity );
	delete m_data;
	m_data = newData;

	m_capacity = newCapacity;
}

void qpString::ShrinkToFit() {
	int fittedCapacity = m_length + 1;
	if ( m_capacity == ( fittedCapacity ) ) {
		return;
	}

	char * fittedData = new char[ fittedCapacity ];
	memset( fittedData, 0, fittedCapacity );
	memcpy_s( fittedData, fittedCapacity, m_data, fittedCapacity );
	delete m_data;
	m_data = fittedData;

	m_capacity = fittedCapacity;
}

void qpString::Clear() {
	memset( m_data, 0, m_capacity );
	m_length = 0;
}

char qpString::At( int index ) {
	qpAssertMsg( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

char qpString::At( int index ) const {
	qpAssertMsg( index < m_length, "Index out of bounds." );
	return m_data[ index ];
}

qpString & qpString::operator+= ( char rhs ) {
	const int oldLength = m_length;
	constexpr int rhsLength = 1;
	Resize( m_length + rhsLength );
	memcpy_s( m_data + oldLength, m_capacity, &rhs, rhsLength );
	return *this;
}

qpString & qpString::operator+= ( const char * rhs ) {
	const int oldLength = m_length;
	const int rhsLength = static_cast < int >( strlen( rhs ) );
	Resize( m_length + rhsLength );
	memcpy_s( m_data + oldLength, m_capacity, rhs, rhsLength );
	return *this;
}

qpString & qpString::operator+= ( const qpString & rhs ) {
	const int oldLength = m_length;
	Resize( m_length + rhs.m_length );
	memcpy_s( m_data + oldLength, m_capacity, rhs.m_data, rhs.m_length );
	return *this;
}

qpString & qpString::operator= ( char rhs ) {
	Assign( rhs );
	return *this;
}

qpString & qpString::operator= ( const char * rhs ) {
	Assign( rhs );
	return *this;
}

qpString & qpString::operator= ( const qpString & rhs ) {
	Assign( rhs );
	return *this;
}

qpString operator+ ( const qpString & lhs, const qpString & rhs ) {
	//@note makes sure data is only allocated once.
	qpString result( lhs.m_length + rhs.m_length + 1 );
	result = lhs;
	result += rhs;
	return result;
}

bool operator== ( const char * lhs, const qpString & rhs ) {
	return !( lhs < rhs ) && !( rhs < lhs );
}

bool operator== ( const qpString & lhs, const char * rhs ) {
	return !( lhs < rhs ) && !( rhs < lhs );
}

bool operator== ( const qpString & lhs, const qpString & rhs ) {
	return !( lhs < rhs ) && !( rhs < lhs );
}

bool operator!= ( const char * lhs, const qpString & rhs ) {
	return !( lhs == rhs );
}

bool operator!= ( const qpString & lhs, const char * rhs ) {
	return !( lhs == rhs );
}

bool operator!= ( const qpString & lhs, const qpString & rhs ) {
	return !( lhs == rhs );
}

bool operator> ( const char * lhs, const qpString & rhs ) {
	return rhs < lhs;
}

bool operator> ( const qpString & lhs, const char * rhs ) {
	return rhs < lhs;
}

bool operator> ( const qpString & lhs, const qpString & rhs ) {
	return rhs < lhs;
}

bool operator>= ( const char * lhs, const qpString & rhs ) {
	return !( lhs < rhs );
}

bool operator>= ( const qpString & lhs, const char * rhs ) {
	return !( lhs < rhs );
}

bool operator>= ( const qpString & lhs, const qpString & rhs ) {
	return !( lhs < rhs );
}

bool operator< ( const char * lhs, const qpString & rhs ) {
	return rhs.Compare( lhs ) < 0;
}

bool operator< ( const qpString & lhs, const char * rhs ) {
	return lhs.Compare( rhs ) < 0;
}

bool operator< ( const qpString & lhs, const qpString & rhs ) {
	return lhs.Compare( rhs ) < 0;
}

bool operator<= ( const char * lhs, const qpString & rhs ) {
	return !( rhs < lhs );
}

bool operator<= ( const qpString & lhs, const char * rhs ) {
	return !( rhs < lhs );
}

bool operator<= ( const qpString & lhs, const qpString & rhs ) {
	return !( rhs < lhs );
}

qpString operator+ ( char lhs, const qpString & rhs ) {
	qpString result( lhs );
	result += rhs;
	return result;
}

qpString operator+ ( const qpString & lhs, char rhs ) {
	qpString result( lhs );
	result += rhs;
	return result;
}

qpString operator+ ( const char * lhs, const qpString & rhs ) {
	qpString result( lhs );
	result += rhs;
	return result;
}

qpString operator+ ( const qpString & lhs, const char * rhs ) {
	qpString result( lhs );
	result += rhs;
	return result;
}

std::ostream & operator<< ( std::ostream & out, const qpString & string ) {
	return out << string.m_data;
}
