#pragma once
#include "qp/common/core/qp_type_traits.h"
#include <cstddef>
#include <compare>

template< typename _type_  >
class qpUniquePtr {
public:
	qpUniquePtr();
	explicit qpUniquePtr( _type_ * ptr );
	qpUniquePtr( nullptr_t null );
	qpUniquePtr( const qpUniquePtr< _type_ > & other ) = delete;
	qpUniquePtr( qpUniquePtr< _type_ > && other ) noexcept;
	template< typename _derived_  >
	qpUniquePtr( qpUniquePtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
	~qpUniquePtr();

	qpUniquePtr< _type_ > & operator=( const qpUniquePtr< _type_ > & rhs ) = delete;
	qpUniquePtr< _type_ > & operator=( nullptr_t null );
	qpUniquePtr< _type_ > & operator=( qpUniquePtr< _type_ > && rhs ) noexcept;
	template< typename _derived_  >
	qpUniquePtr< _type_ > & operator=( qpUniquePtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > );

	auto operator<=>( const qpUniquePtr< _type_ > & rhs ) const;
	auto operator<=>( const _type_ * rhs ) const;
	bool operator==( const qpUniquePtr< _type_ > & rhs ) const;
	bool operator==( const _type_ * rhs ) const;

	_type_ * operator->() { return m_ptr; }
	const _type_ * operator->() const { return m_ptr; }

	_type_ & operator*() { return *m_ptr; }
	const _type_ & operator*() const { return *m_ptr; }

	_type_ * Raw() const { return m_ptr; }
	_type_ * Release();

	void Reset();
	void Reset( _type_ * data );
private:
	_type_ * m_ptr = NULL;
};

template< typename _type_  >
qpUniquePtr< _type_ >::qpUniquePtr() {
}

template< typename _type_  >
qpUniquePtr< _type_ >::qpUniquePtr( _type_ * ptr ) {
	m_ptr = ptr;
}

template< typename _type_  >
qpUniquePtr< _type_ >::qpUniquePtr( nullptr_t null ) {
	m_ptr = null;
}

template< typename _type_  >
qpUniquePtr< _type_ >::qpUniquePtr( qpUniquePtr< _type_ > && other ) noexcept {
	m_ptr = other.Release();
}

template< typename _type_  >
template< typename _derived_  >
qpUniquePtr< _type_ >::qpUniquePtr( qpUniquePtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
	m_ptr = other.Release();
}

template< typename _type_  >
qpUniquePtr< _type_ >::~qpUniquePtr() {
	delete m_ptr;
}

template< typename _type_  >
qpUniquePtr< _type_ > & qpUniquePtr< _type_ >::operator=( qpUniquePtr< _type_ > && rhs ) noexcept {
	delete m_ptr;
	m_ptr = rhs.Release();
	return *this;
}

template< typename _type_  >
qpUniquePtr< _type_ > & qpUniquePtr< _type_ >::operator=( nullptr_t null ) {
	delete m_ptr;
	m_ptr = null;
	return *this;
}

template< typename _type_  >
template< typename _derived_  >
qpUniquePtr< _type_ > & qpUniquePtr< _type_ >::operator=( qpUniquePtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
	delete m_ptr;
	m_ptr = rhs.Release();
	return *this;
}

template< typename _type_  >
auto qpUniquePtr< _type_ >::operator<=>( const qpUniquePtr< _type_ > & rhs ) const {
	return ( m_ptr - rhs.m_ptr );
}

template< typename _type_  >
auto qpUniquePtr< _type_ >::operator<=>( const _type_ * rhs ) const {
	return ( m_ptr - rhs );
}

template< typename _type_  >
bool qpUniquePtr< _type_ >::operator==( const qpUniquePtr< _type_ > & rhs ) const {
	return m_ptr == rhs.m_ptr;
}

template< typename _type_  >
bool qpUniquePtr< _type_ >::operator==( const _type_ * rhs ) const {
	return m_ptr == rhs;
}

template< typename _type_  >
_type_ * qpUniquePtr< _type_ >::Release() {
	_type_ * ptr = m_ptr;
	m_ptr = NULL;
	return ptr;
}

template< typename _type_  >
void qpUniquePtr< _type_ >::Reset() {
	return Reset( NULL );
}

template< typename _type_  >
void qpUniquePtr< _type_ >::Reset( _type_ * data ) {
	delete m_ptr;
	m_ptr = data;
}

template < typename _type_ , typename ... _args_ >
constexpr static inline qpUniquePtr< _type_ > qpCreateUnique( _args_&&... args ) {
	return qpUniquePtr< _type_ >( new _type_( qpForward< _args_ >( args )... ) );
}