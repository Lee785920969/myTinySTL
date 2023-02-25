#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

#include <new>

#include"my_type_traits.h"
#include"my_iterator.h"

#if  _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif //  _MSC_VER


namespace mystl
{
	template<class T1>
	void construct(T1* ptr)
	{
		::new ((void*)ptr) T1();
	}

	template<class T1, class T2>
	void construct(T1* p, const T2& value)
	{
		::new ((void*)p)T1(value);
	}


	template <class T, class ...Arg>
	void construct(T* p, Arg&& ...arg)
	{
		::new ((void*)p) T(mystl::forward<Arg>(arg)...);
	}


	template <class T>
	void destroy_one(T*, std::true_type) {};

	template <class T>
	void destroy_one(T* pointer, std::false_type)
	{
		if (pointer != nullptr)
		{
			pointer -> ~T();
		}
	}

	template<class ForwardIter>
	void destroy_cat(ForwardIter, ForwardIter, std::true_type) {};

	template<class ForwardIter>
	void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
	{
		for (; first != last; ++first)
			destroy(&*first);
	}

	template < class T>
	void detroy(T* pointer)
	{
		destroy_one(pointer, std::is_trivially_destructible<T>{});
	}

	template<class ForwardIter>
	void destroy(ForwardIter first, ForwardIter last)
	{
		destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type{});
	}


}


#ifdef _MSC_VER
#pragma warning(pop)
#endif 

#endif // !MYTINYSTL_CONSTRUCT_H_

