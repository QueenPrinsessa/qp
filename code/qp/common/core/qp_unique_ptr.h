#pragma once
#include "qp/common/core/qp_type_traits.h"
#include <cstddef>
#include <compare>

namespace qp {
	template< typename _type_  >
	class UniquePtr {
	public:
		UniquePtr();
		explicit UniquePtr( _type_ * ptr );
		UniquePtr( nullptr_t null );
		UniquePtr( const UniquePtr< _type_ > & other ) = delete;
		UniquePtr( UniquePtr< _type_ > && other ) noexcept;
		template< typename _derived_  >
		UniquePtr( UniquePtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
		~UniquePtr();

		UniquePtr< _type_ > & operator=( const UniquePtr< _type_ > & rhs ) = delete;
		UniquePtr< _type_ > & operator=( nullptr_t null );
		UniquePtr< _type_ > & operator=( UniquePtr< _type_ > && rhs ) noexcept;
		template< typename _derived_  >
		UniquePtr< _type_ > & operator=( UniquePtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > );

		auto operator<=>( const UniquePtr< _type_ > & rhs ) const;
		auto operator<=>( const _type_ * rhs ) const;
		bool operator==( const UniquePtr< _type_ > & rhs ) const;
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
	UniquePtr< _type_ >::UniquePtr() {
	}

	template< typename _type_  >
	UniquePtr< _type_ >::UniquePtr( _type_ * ptr ) {
		m_ptr = ptr;
	}

	template< typename _type_  >
	UniquePtr< _type_ >::UniquePtr( nullptr_t null ) {
		m_ptr = null;
	}

	template< typename _type_  >
	UniquePtr< _type_ >::UniquePtr( UniquePtr< _type_ > && other ) noexcept {
		m_ptr = other.Release();
	}

	template< typename _type_  >
	template< typename _derived_  >
	UniquePtr< _type_ >::UniquePtr( UniquePtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
		m_ptr = other.Release();
	}

	template< typename _type_  >
	UniquePtr< _type_ >::~UniquePtr() {
		delete m_ptr;
	}

	template< typename _type_  >
	UniquePtr< _type_ > & UniquePtr< _type_ >::operator=( UniquePtr< _type_ > && rhs ) noexcept {
		delete m_ptr;
		m_ptr = rhs.Release();
		return *this;
	}

	template< typename _type_  >
	UniquePtr< _type_ > & UniquePtr< _type_ >::operator=( nullptr_t null ) {
		delete m_ptr;
		m_ptr = null;
		return *this;
	}

	template< typename _type_  >
	template< typename _derived_  >
	UniquePtr< _type_ > & UniquePtr< _type_ >::operator=( UniquePtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
		delete m_ptr;
		m_ptr = rhs.Release();
		return *this;
	}

	template< typename _type_  >
	auto UniquePtr< _type_ >::operator<=>( const UniquePtr< _type_ > & rhs ) const {
		return ( m_ptr - rhs.m_ptr );
	}

	template< typename _type_  >
	auto UniquePtr< _type_ >::operator<=>( const _type_ * rhs ) const {
		return ( m_ptr - rhs );
	}

	template< typename _type_  >
	bool UniquePtr< _type_ >::operator==( const UniquePtr< _type_ > & rhs ) const {
		return m_ptr == rhs.m_ptr;
	}

	template< typename _type_  >
	bool UniquePtr< _type_ >::operator==( const _type_ * rhs ) const {
		return m_ptr == rhs;
	}

	template< typename _type_  >
	_type_ * UniquePtr< _type_ >::Release() {
		_type_ * ptr = m_ptr;
		m_ptr = NULL;
		return ptr;
	}

	template< typename _type_  >
	void UniquePtr< _type_ >::Reset() {
		return Reset( NULL );
	}

	template< typename _type_  >
	void UniquePtr< _type_ >::Reset( _type_ * data ) {
		delete m_ptr;
		m_ptr = data;
	}

	template < typename _type_, typename ... _args_ >
	constexpr static inline UniquePtr< _type_ > CreateUnique( _args_&&... args ) {
		return UniquePtr< _type_ >( new _type_( Forward< _args_ >( args )... ) );
	}
}