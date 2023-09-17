#pragma once
#include <cstddef>
#include <compare>

template< typename T >
class qpUniquePtr {
public:
	qpUniquePtr();
	explicit qpUniquePtr( T * ptr );
	qpUniquePtr( nullptr_t null );
	qpUniquePtr( const qpUniquePtr< T > & other ) = delete;
	qpUniquePtr( qpUniquePtr< T > && other ) noexcept;
	template< typename D >
	qpUniquePtr( qpUniquePtr< D > && other ) noexcept requires ( std::is_base_of_v< T, D > );
	~qpUniquePtr();

	qpUniquePtr< T > & operator=( const qpUniquePtr< T > & rhs ) = delete;
	qpUniquePtr< T > & operator=( nullptr_t null );
	qpUniquePtr< T > & operator=( qpUniquePtr< T > && rhs ) noexcept;
	template< typename D >
	qpUniquePtr< T > & operator=( qpUniquePtr< D > && rhs ) noexcept requires ( std::is_base_of_v< T, D > );

	auto operator<=>( const qpUniquePtr< T > & rhs ) const;
	auto operator<=>( const T * rhs ) const;
	bool operator==( const qpUniquePtr< T > & rhs ) const;
	bool operator==( const T * rhs ) const;

	T * operator->() { return m_ptr; }
	const T * operator->() const { return m_ptr; }

	T & operator*() { return *m_ptr; }
	const T & operator*() const { return *m_ptr; }

	T * Raw() const { return m_ptr; }
	T * Release();

	void Reset();
	void Reset( T * data );
private:
	T * m_ptr = NULL;
};

template< typename T >
qpUniquePtr< T >::qpUniquePtr() {
}

template< typename T >
qpUniquePtr< T >::qpUniquePtr( T * ptr ) {
	m_ptr = ptr;
}

template< typename T >
qpUniquePtr< T >::qpUniquePtr( nullptr_t null ) {
	m_ptr = null;
}

template< typename T >
qpUniquePtr< T >::qpUniquePtr( qpUniquePtr< T > && other ) noexcept {
	m_ptr = other.Release();
}

template< typename T >
template< typename D >
qpUniquePtr< T >::qpUniquePtr( qpUniquePtr< D > && other ) noexcept requires ( std::is_base_of_v< T, D > ) {
	m_ptr = other.Release();
}

template< typename T >
qpUniquePtr< T >::~qpUniquePtr() {
	delete m_ptr;
}

template< typename T >
qpUniquePtr< T > & qpUniquePtr< T >::operator=( qpUniquePtr< T > && rhs ) noexcept {
	delete m_ptr;
	m_ptr = rhs.Release();
	return *this;
}

template< typename T >
qpUniquePtr< T > & qpUniquePtr< T >::operator=( nullptr_t null ) {
	delete m_ptr;
	m_ptr = null;
	return *this;
}

template< typename T >
template< typename D >
qpUniquePtr< T > & qpUniquePtr< T >::operator=( qpUniquePtr< D > && rhs ) noexcept requires ( std::is_base_of_v< T, D > ) {
	delete m_ptr;
	m_ptr = rhs.Release();
	return *this;
}

template< typename T >
auto qpUniquePtr< T >::operator<=>( const qpUniquePtr< T > & rhs ) const {
	return ( m_ptr - rhs.m_ptr );
}

template< typename T >
auto qpUniquePtr< T >::operator<=>( const T * rhs ) const {
	return ( m_ptr - rhs );
}

template< typename T >
bool qpUniquePtr< T >::operator==( const qpUniquePtr< T > & rhs ) const {
	return m_ptr == rhs.m_ptr;
}

template< typename T >
bool qpUniquePtr< T >::operator==( const T * rhs ) const {
	return m_ptr == rhs;
}

template< typename T >
T * qpUniquePtr< T >::Release() {
	T * ptr = m_ptr;
	m_ptr = NULL;
	return ptr;
}

template< typename T >
void qpUniquePtr< T >::Reset() {
	return Reset( NULL );
}

template< typename T >
void qpUniquePtr< T >::Reset( T * data ) {
	delete m_ptr;
	m_ptr = data;
}

template < typename T, typename ... ARGS >
constexpr static inline qpUniquePtr< T > qpCreateUnique( ARGS&&... args ) {
	return qpUniquePtr< T >( new T( std::forward< ARGS >( args )... ) );
}