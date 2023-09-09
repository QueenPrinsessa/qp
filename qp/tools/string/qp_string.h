#pragma once
#include <cstddef>
#include <ostream>

class qpString {
public:
	qpString();
	explicit qpString( int capacity );
	qpString( char c );
	qpString( const char * string );
	qpString( const qpString & other );
	qpString( qpString && other ) noexcept;
	~qpString();

	qpString & Assign( const char * string, int length );
	qpString & Assign( char c );
	qpString & Assign( const char * string );
	qpString & Assign( const qpString & string );

	int Compare( const char * string ) const;
	int Compare( const qpString & string ) const;

	void Resize( int newLength, char charToInsert );
	void Resize( int newLength );
	void Reserve( int newCapacity );
	void ShrinkToFit();

	void Clear();

	char At( int index );
	char At( int index ) const;

	int Length() const { return m_length; }
	int Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0; }

	const char * Data() const { return m_data; }
	const char * c_str() const { return m_data; }

	qpString & operator+= ( char rhs );
	qpString & operator+= ( const char * rhs );
	qpString & operator+= ( const qpString & rhs );
	qpString & operator= ( char rhs );
	qpString & operator= ( const char * rhs );
	qpString & operator= ( const qpString & rhs );

	friend bool operator== ( const char * lhs, const qpString & rhs );
	friend bool operator== ( const qpString & lhs, const char * rhs );
	friend bool operator== ( const qpString & lhs, const qpString & rhs );
	friend bool operator!= ( const char * lhs, const qpString & rhs );
	friend bool operator!= ( const qpString & lhs, const char * rhs );
	friend bool operator!= ( const qpString & lhs, const qpString & rhs );
	friend bool operator> ( const char * lhs, const qpString & rhs );
	friend bool operator> ( const qpString & lhs, const char * rhs );
	friend bool operator> ( const qpString & lhs, const qpString & rhs );
	friend bool operator>= ( const char * lhs, const qpString & rhs );
	friend bool operator>= ( const qpString & lhs, const char * rhs );
	friend bool operator>= ( const qpString & lhs, const qpString & rhs );
	friend bool operator< ( const char * lhs, const qpString & rhs );
	friend bool operator< ( const qpString & lhs, const char * rhs );
	friend bool operator< ( const qpString & lhs, const qpString & rhs );
	friend bool operator<= ( const char * lhs, const qpString & rhs );
	friend bool operator<= ( const qpString & lhs, const char * rhs );
	friend bool operator<= ( const qpString & lhs, const qpString & rhs );
	friend qpString operator+( char lhs, const qpString & rhs );
	friend qpString operator+( const qpString & lhs, char rhs );
	friend qpString operator+( const char * lhs, const qpString & rhs );
	friend qpString operator+( const qpString & lhs, const char * rhs );
	friend qpString operator+( const qpString & lhs, const qpString & rhs );
	friend std::ostream & operator<< ( std::ostream & out, const qpString & string );
private:
	int m_capacity = 0;
	int m_length = 0;
	char * m_data = NULL;
};

