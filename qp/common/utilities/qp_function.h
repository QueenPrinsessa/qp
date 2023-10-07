#pragma once
#include "qp/common/core/qp_smart_pointers.h"
#include "qp/common/debug/qp_debug.h"

template < typename >
class qpFunction;

template < typename R, typename ... ARGS >
class qpFunction< R( ARGS... ) > {
public:
	qpFunction() = default;
	qpFunction( nullptr_t ){}
	template < typename T >
	qpFunction( T func );

	qpFunction( const qpFunction & other ) noexcept;

	qpFunction( qpFunction && other ) noexcept;
	~qpFunction() = default;

	template < typename T >
	qpFunction & operator=( T func );
	qpFunction & operator=( nullptr_t );
	qpFunction & operator=( const qpFunction & rhs );
	qpFunction & operator=( qpFunction && rhs ) noexcept;

	bool operator==( nullptr_t ) { return m_functor != NULL; }

	R operator()( ARGS ... args ) const;

	operator bool() const { return m_functor != NULL; }
private:
	class qpFunctor
	{
	public:
		virtual ~qpFunctor() = default;
		virtual R Invoke( ARGS... ) const = 0;

		QP_INTRUSIVE_REF_COUNTER;
	};
	template < typename T >
	class qpTFunctor : public qpFunctor
	{
	public:
		qpTFunctor( const T & func ) : m_func( func ) { }
		virtual ~qpTFunctor() override = default;
		virtual R Invoke( ARGS ... args ) const override { return m_func( args... ); }
	private:
		T m_func;
	};

	qpIntrusiveRefPtr< qpFunctor > m_functor;
};

template< typename R, typename ... ARGS >
template< typename T >
qpFunction< R( ARGS... ) >::qpFunction( T func ) {
	m_functor = qpCreateIntrusiveRef< qpTFunctor< T > >( func );
}

template< typename R, typename ... ARGS >
qpFunction< R( ARGS... ) >::qpFunction( const qpFunction & other ) noexcept {
	m_functor = other.m_functor;
}

template< typename R, typename ... ARGS >
qpFunction< R( ARGS... ) >::qpFunction( qpFunction && other ) noexcept {
	m_functor = qpMove( other.m_functor );
}

template< typename R, typename ... ARGS >
template< typename T >
qpFunction< R( ARGS... ) > & qpFunction< R( ARGS... ) >::operator=( T func ) {
	m_functor = qpCreateIntrusiveRef< qpTFunctor< T > >( func );
	return *this;
}

template< typename R, typename ... ARGS >
qpFunction< R( ARGS... ) > & qpFunction< R( ARGS... ) >::operator=( nullptr_t ) {
	m_functor.Reset();
	return *this;
}

template< typename R, typename ... ARGS >
qpFunction< R( ARGS... ) > & qpFunction< R( ARGS... ) >::operator=( const qpFunction & rhs ) {
	m_functor = rhs.m_functor;
	return *this;
}

template< typename R, typename ... ARGS >
qpFunction< R( ARGS... ) > & qpFunction< R( ARGS... ) >::operator=( qpFunction && rhs ) noexcept {
	m_functor = qpMove( rhs.m_functor );
	return *this;
}

template< typename R, typename ... ARGS >
R qpFunction< R( ARGS... ) >::operator()( ARGS... args ) const {
	QP_ASSERT( m_functor != NULL );
	return m_functor->Invoke( args... );
}
