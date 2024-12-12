#pragma once
#include "qp_types.h"

#define QP_INTRUSIVE_INCREMENT_REF IncrementIntrusiveRef
#define QP_INTRUSIVE_DECREMENT_REF DecrementIntrusiveRef
#define QP_INTRUSIVE_GET_COUNTER GetIntrusiveRefCount
#define QP_INTRUSIVE_COUNTER_MEMBER m_intrusiveRefCounter

#define QP_INTRUSIVE_REF_COUNTER \
public: \
	void QP_INTRUSIVE_INCREMENT_REF() const { ++QP_INTRUSIVE_COUNTER_MEMBER; } \
	void QP_INTRUSIVE_DECREMENT_REF() const { --QP_INTRUSIVE_COUNTER_MEMBER; } \
	qp::uint32 QP_INTRUSIVE_GET_COUNTER() const { return QP_INTRUSIVE_COUNTER_MEMBER.load(); } \
private: \
	mutable qp::atomicUInt32_t QP_INTRUSIVE_COUNTER_MEMBER = 0u

namespace qp {
	template< typename _type_ >
	constexpr bool HasIntrusiveRefCounter = (
		requires ( const _type_ & t ) {
		t.QP_INTRUSIVE_INCREMENT_REF();
	} && requires ( const _type_ & t ) {
		t.QP_INTRUSIVE_DECREMENT_REF();
	} && requires ( const _type_ & t ) {
		t.QP_INTRUSIVE_GET_COUNTER();
	} );

	template < typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	class IntrusiveRefPtr {
	public:
		IntrusiveRefPtr();
		explicit IntrusiveRefPtr( _type_ * ptr );
		IntrusiveRefPtr( nullptr_t );
		IntrusiveRefPtr( const IntrusiveRefPtr & other );
		IntrusiveRefPtr( IntrusiveRefPtr && other ) noexcept;
		template< typename _derived_ >
		IntrusiveRefPtr( const IntrusiveRefPtr< _derived_ > & other ) requires ( IsBaseOf< _type_, _derived_ > );
		template< typename _derived_ >
		IntrusiveRefPtr( IntrusiveRefPtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
		~IntrusiveRefPtr();

		IntrusiveRefPtr & operator=( const IntrusiveRefPtr & rhs );
		IntrusiveRefPtr & operator=( IntrusiveRefPtr && rhs ) noexcept;
		template< typename _derived_ >
		IntrusiveRefPtr & operator=( const IntrusiveRefPtr< _derived_ > & rhs ) requires ( IsBaseOf< _type_, _derived_ > );
		template< typename _derived_ >
		IntrusiveRefPtr & operator=( IntrusiveRefPtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
		IntrusiveRefPtr & operator=( nullptr_t null );

		auto operator<=>( const IntrusiveRefPtr & rhs ) const;
		auto operator<=>( const _type_ * rhs ) const;
		bool operator==( const IntrusiveRefPtr & rhs ) const;
		bool operator==( const _type_ * rhs ) const;

		_type_ * operator->() { return m_ptr; }
		const _type_ * operator->() const { return m_ptr; }
		_type_ & operator*() { return *m_ptr; }
		const _type_ & operator*() const { return *m_ptr; }

		_type_ * Raw() const { return m_ptr; }

		void Reset();
		void Reset( _type_ * data );

		uint32 GetRefCount() const { return m_ptr ? m_ptr->QP_INTRUSIVE_GET_COUNTER() : 0; }

	private:
		void IncrementRef();
		void DecrementRef();

		_type_ * m_ptr = NULL;
	};

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr() {
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( _type_ * ptr ) {
		m_ptr = ptr;
		IncrementRef();
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( nullptr_t null ) {
		m_ptr = null;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( const IntrusiveRefPtr & other ) {
		m_ptr = other.m_ptr;
		IncrementRef();
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( IntrusiveRefPtr && other ) noexcept {
		m_ptr = other.m_ptr;
		other.m_ptr = NULL;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	template< typename _derived_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( const IntrusiveRefPtr< _derived_ > & other ) requires ( IsBaseOf< _type_, _derived_ > ) {
		m_ptr = other.Raw();
		IncrementRef();
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	template< typename _derived_ >
	IntrusiveRefPtr< _type_ >::IntrusiveRefPtr( IntrusiveRefPtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
		m_ptr = other.Raw();
		IncrementRef();
		other.Reset( NULL );
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ >::~IntrusiveRefPtr() {
		DecrementRef();
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ > & IntrusiveRefPtr< _type_ >::operator=( const IntrusiveRefPtr & rhs ) {
		m_ptr = rhs.m_ptr;
		IncrementRef();
		return *this;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ > & IntrusiveRefPtr< _type_ >::operator=( IntrusiveRefPtr && rhs ) noexcept {
		m_ptr = rhs.m_ptr;
		rhs.m_ptr = NULL;
		return *this;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	IntrusiveRefPtr< _type_ > & IntrusiveRefPtr< _type_ >::operator=( nullptr_t null ) {
		DecrementRef();
		m_ptr = null;
		return *this;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	template< typename _derived_ >
	IntrusiveRefPtr< _type_ > & IntrusiveRefPtr< _type_ >::operator=( const IntrusiveRefPtr < _derived_ > & rhs ) requires ( IsBaseOf< _type_, _derived_ > ) {
		m_ptr = rhs.Raw();
		IncrementRef();
		return *this;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	template< typename _derived_ >
	IntrusiveRefPtr< _type_ > & IntrusiveRefPtr< _type_ >::operator=( IntrusiveRefPtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
		m_ptr = rhs.Raw();
		IncrementRef();
		rhs.Reset( NULL );
		return *this;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	auto IntrusiveRefPtr< _type_ >::operator<=>( const IntrusiveRefPtr & rhs ) const {
		return ( m_ptr - rhs.m_ptr );
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	auto IntrusiveRefPtr< _type_ >::operator<=>( const _type_ * rhs ) const {
		return ( m_ptr - rhs );
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	bool IntrusiveRefPtr< _type_ >::operator==( const IntrusiveRefPtr & rhs ) const {
		return m_ptr == rhs.m_ptr;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	bool IntrusiveRefPtr< _type_ >::operator==( const _type_ * rhs ) const {
		return m_ptr == rhs;
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	void IntrusiveRefPtr< _type_ >::Reset() {
		Reset( NULL );
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	void IntrusiveRefPtr< _type_ >::Reset( _type_ * data ) {
		DecrementRef();
		m_ptr = data;
		IncrementRef();
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	void IntrusiveRefPtr< _type_ >::IncrementRef() {
		if ( m_ptr ) {
			m_ptr->QP_INTRUSIVE_INCREMENT_REF();
		}
	}

	template< typename _type_ > requires HasIntrusiveRefCounter< _type_ >
	void IntrusiveRefPtr< _type_ >::DecrementRef() {
		if ( m_ptr ) {
			m_ptr->QP_INTRUSIVE_DECREMENT_REF();

			if ( m_ptr->QP_INTRUSIVE_GET_COUNTER() == 0 ) {
				delete m_ptr;
				m_ptr = NULL;
			}
		}
	}

	template < typename _type_, typename ... _args_ >
	constexpr static inline IntrusiveRefPtr< _type_ > CreateIntrusiveRef( _args_&&... args ) {
		return IntrusiveRefPtr< _type_ >( new _type_( Forward< _args_ >( args )... ) );
	}

	template < typename _type_, typename _derived_ >
	constexpr static inline IntrusiveRefPtr< _type_ > IntrusiveRefStaticCast( const IntrusiveRefPtr< _derived_ > & intrusivePtr ) {
		return IntrusiveRefPtr< _type_ >( static_cast< _type_ * >( intrusivePtr.Raw() ) );
	}

	template < typename _type_, typename _derived_ >
	constexpr static inline IntrusiveRefPtr< _type_ > IntrusiveRefDynamicCast( const IntrusiveRefPtr< _derived_ > & intrusivePtr ) {
		return IntrusiveRefPtr< _type_ >( dynamic_cast< _type_ * >( intrusivePtr.Raw() ) );
	}

	template < typename _type_, typename _derived_ >
	constexpr static inline IntrusiveRefPtr< _type_ > IntrusiveRefReinterpretCast( const IntrusiveRefPtr< _derived_ > & intrusivePtr ) {
		return IntrusiveRefPtr< _type_ >( reinterpret_cast< _type_ * >( intrusivePtr.Raw() ) );
	}

	template < typename _type_, typename _derived_ >
	constexpr static inline IntrusiveRefPtr< _type_ > IntrusiveRefConstCast( const IntrusiveRefPtr< _derived_ > & intrusivePtr ) {
		return IntrusiveRefPtr< _type_ >( const_cast< _type_ * >( intrusivePtr.Raw() ) );
	}
}