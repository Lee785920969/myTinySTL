#ifndef  MYTINYSTL_UTIL_H_
#define  MYTINYSTL_UTIL_H_

#include <cstddef>
#include"my_type_traits.h"


namespace mystl
{
	//move;

	template <class T>
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept
	{
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}

	//forward

	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept
	{
		return static_cast<T&&>(arg);
	}

	template <class T>
	T&& forward(typename std::remove_reference<T>::type&& arg)noexcept
	{
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
		return static_cast<T&&> (arg);
	}

	//swap

	template <class T>
	void swap(T& lhs, T& rhs)
	{
		auto tmp(mystl::move(lhs));
		lhs = mystl::move(rhs);
		rhs = mystl::move(tmp);
	}


	template <class ForwardIter1, class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
	{
		for (; first1 != last1; ++first1, (void) ++first2)
			mystl::swap(*first1, *first2);
		return first2;
	}


	template <class Tp , size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N])
	{
		mystl::swap_range(a, a + N, b);
	}

	//------------------------pair--------------//

	// 结构体模板：pair


	template <class Ty1, class Ty2>
	struct pair
	{
		typedef Ty1             first_type;
		typedef Ty2             second_type;

		first_type    first;
		second_type   second;

		template <class other1 = Ty1 , class other2 = Ty2 , 
			typename = typename std::enable_if <
			std::is_default_constructible<other1>::value &&
			std::is_default_constructible<other2>::value, void>::type>
		constexpr pair() : first(), second()
		{

		}



		template <class u1 = Ty1, class u2 = Ty2, 
			typename std::enable_if<
			std::is_copy_constructible<u1>::value &&
			std::is_copy_constructible<u2>::value &&
			std::is_convertible<u1& , Ty1> && std::is_convertible<u2& , Ty2> , int> ::type = 0>
		constexpr pair(const Ty1& a, const Ty2& b) : first(a), first(b)
		{
		}



		template <class u1 = Ty1, class u2 = Ty2,
			typename std::enable_if<
			std::is_copy_constructible<u1>::value&&
			std::is_copy_constructible<u2>::value&&
			(!std::is_convertible<u1&, Ty1>|| std::is_convertible<u2&, Ty2>), int> ::type = 0>
		constexpr pair(const Ty1& a, const Ty2& b) : first(a), first(b)
		{
		}


		pair(const pair& rhs) = default;
		pair(pair&& rhs) = default;


		template <class other1 , class other2,
			typename std::enable_if<
			std::is_constructible<Ty1, other1>::value &&
			std::is_constructible<Ty2, other2>::value &&
			std::is_convertible<other1&& , Ty1>::value&&
			std::is_convertible<other2&&, Ty2>::value,int>::type = 0 >
		constexpr pair(other1&& a, other2&& b) :
			first(mystl::forward<other1>(a)), second(mystl::forward<other2>(b))
		{
		}



		template <class other1, class other2,
			typename std::enable_if<
			std::is_constructible<Ty1, other1>::value&&
			std::is_constructible<Ty2, other2>::value&&
			(!std::is_convertible<other1&&, Ty1>::value||
			!std::is_convertible<other2&&, Ty2>::value), int>::type = 0 >
		constexpr pair(other1&& a, other2&& b) :
			first(mystl::forward<other1>(a)), second(mystl::forward<other2>(b))
		{
		}


		template<class other1 , class other2,
			typename std:: enable_if<
			std::is_constructible<Ty1 , const other1&>::value&&
			std::is_constructible<Ty2 , const other2&>::value&&
			std::is_convertible<const other1&, Ty1>::value &&
			std::is_convertible<const other2&, Ty2>::value , int>::type = 0>
		constexpr pair(const pair<other1.other2>& other)
			: first(other.first), second(other.second)
		{};


		template<class other1 , class other2,
			typename std::enable_if<
			std::is_constructible<Ty1 , other1>::value &&
			std::is_constructible<Ty2 , other2>::value &&
			(!std::is_convertible<const other1&, Ty1>::value ||
			!std::is_convertible<const other2&, Ty2>::value), int>::type = 0>
		constexpr pair(const pair<other1.other2>& other)
			: first(other.first), second(other.second)
		{};

		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_convertible<Other1, Ty1>::value&&
			std::is_convertible<Other2, Ty2>::value, int>::type = 0>
		constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
		{
		}

		// explicit constructiable for other pair
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value &&
			(!std::is_convertible<Other1, Ty1>::value ||
				!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
		explicit constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
		{
		}



		pair& operator=(const pair& rhs)
		{
			if (this != &rhs)
			{
				first = rhs.first;
				second = rhs.second;
			}
			return *this;
		}


		pair& operator=(pair&& rhs)
		{
			if (this != &rhs)
			{
				first = mystl::move(rhs.first);
				second = mystl::move(rhs.second);
			}

			return *this;
		}


		template <class other1 , class other2>
		pair& operator = (pair<other1, other2>& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}

		template <class other1 , class other2 >
		pair& operator=(pair<other1, other2>&& other)
		{
			first = mystl::forward(other.first);
			second = mystrl::forward(other.second);
			return *this;
		}

		~pair() = default;

		void swap(pair& other)
		{
			if (this != &other)
			{
				mystl::swap(first, other.first);
				mystl::swap(second, other.second);
			}

		 }


	};

	template <class T1,class T2>
	bool operator == (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}


	template <class T1, class T2>
	bool operator < (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
	}


	template <class T1, class T2>
	bool operator != (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T1, class T2>
	bool operator > (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return rhs < lhs;
	}

	template <class T1, class T2>
	bool operator >= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T1, class T2>
	bool operator <= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return !(rhs < lhs);
	}


	template<class T1, class T2>
	void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs)
	{
		lhs.swap(rhs);
	}

	template<class T1, class T2>
	pair<T1, T2> make_pair(T1&& first, T2&& second)
	{
		return pair<T1, T2>(mystl::forward<T1>(first), mystl::forward<T2>(second));
	}

}




#endif // ! MYTINYSTL_UTIL_H_

