#pragma once
#include <cstddef>
#include <iterator>

template < typename _type_ >
class qpList;

template < typename _type_, int _size_ >
class qpStaticList;

template < typename _type_, int _size_ >
class qpArray;

template < typename _type_ >
class qpArrayView {
public:
	struct ConstIterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = _type_;
		using pointer = const _type_ *;
		using reference = const _type_ &;

		ConstIterator( pointer ptr ) : m_ptr( ptr ) {}

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

		auto operator<=>( const qpArrayView::ConstIterator & ) const = default;
		bool operator==( const qpArrayView::ConstIterator & ) const = default;
	private:
		pointer m_ptr = NULL;
	};
	qpArrayView( const qpList< _type_ > & list );
	template < int _size_ >
	explicit qpArrayView( const qpStaticList< _type_, _size_ > & list );
	template < int _size_ >
	qpArrayView( const qpArray< _type_, _size_ > & arr );

	int Length() const { return m_length; }
	const _type_ * Data() const { return m_ptr; }

	ConstIterator Begin() const { return ConstIterator( &m_ptr[ 0 ] ); }
	ConstIterator End() const { return ConstIterator( &m_ptr[ m_length ] ); }
	ConstIterator begin() const { return Begin(); }
	ConstIterator end() const { return End(); }
private:
	int m_length = 0;
	const _type_ * m_ptr = NULL;
};

template< typename _type_ >
qpArrayView< _type_ >::qpArrayView( const qpList< _type_ > & list ) {
	m_ptr = list.m_data;
	m_length = list.Length();
}

template< typename _type_ >
template< int _size_ >
qpArrayView< _type_ >::qpArrayView( const qpStaticList<_type_, _size_> & list ) {
	m_ptr = list.m_data;
	m_length = list.Length();
}

template< typename _type_ >
template< int _size_ >
qpArrayView<_type_>::qpArrayView( const qpArray< _type_, _size_ > & arr ) {
	m_ptr = &arr.m_data[ 0 ];
	m_length = arr.Length();
}
