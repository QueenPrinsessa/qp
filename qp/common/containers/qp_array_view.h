#pragma once
#include <cstddef>
#include <iterator>

template < typename T >
class qpList;

template < typename T, int SIZE >
class qpStaticList;

template < typename T, int SIZE >
class qpArray;

template < typename T >
class qpArrayView {
public:
	struct ConstIterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = const T *;
		using reference = const T &;

		ConstIterator( pointer ptr ) : m_ptr( ptr ) { }

		reference operator *() const { return *m_ptr; }

		pointer operator->() { return m_ptr; }
		ConstIterator & operator++() {
			m_ptr++;
			return *this;
		}
		ConstIterator operator++( int ) {
			ConstIterator it = m_ptr++;
			return it;
		}

		auto operator<=>( const qpArrayView< T >::ConstIterator & ) const = default;
		bool operator==( const qpArrayView< T >::ConstIterator & ) const = default;
	private:
		pointer m_ptr = NULL;
	};
	qpArrayView( const qpList< T > & list );
	template < int SIZE >
	explicit qpArrayView( const qpStaticList< T, SIZE > & list );
	template < int SIZE >
	qpArrayView( const qpArray< T, SIZE > & arr );

	int Length() const { return m_length; }
	const T * Data() const { return m_ptr; }

	ConstIterator Begin() const { return ConstIterator( &m_ptr[ 0 ] ); }
	ConstIterator End() const { return ConstIterator( &m_ptr[ m_length ] ); }
	ConstIterator begin() const { return Begin(); }
	ConstIterator end() const { return End(); }
private:
	int m_length = 0;
	const T * m_ptr = NULL;
};

template< typename T >
qpArrayView<T>::qpArrayView( const qpList< T > & list ) {
	m_ptr = list.m_data;
	m_length = list.Length();
}

template< typename T >
template< int SIZE >
qpArrayView<T>::qpArrayView( const qpStaticList<T, SIZE> & list ) {
	m_ptr = list.m_data;
	m_length = list.Length();
}

template< typename T >
template< int SIZE >
qpArrayView<T>::qpArrayView( const qpArray< T, SIZE > & arr ) {
	m_ptr = &arr.m_data[ 0 ];
	m_length = arr.Length();
}
