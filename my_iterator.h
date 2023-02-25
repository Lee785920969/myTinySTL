#ifndef  MY_ITERATOR_H_
#define  MY_ITERATOR_H_

#include<cstddef>
#include"my_type_traits.h"


namespace mystl
{
	//五种迭代器
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag{};
	struct bidirectional_iterator_tag : public forward_iterator_tag{};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	//迭代器模板

	template< class Category ,class T , class Distance = ptrdiff_t, class Pointer =  T*, class Reference = T&>
	struct iterator
	{
		typedef Category        iterator_category;
		typedef T               value_type;
		typedef Pointer         pointer;
		typedef Reference       reference;
		typedef Distance        difference_type;
	};

	// iterator traits

	template <class T>
	struct has_iterator_cat
	{
	private:
		struct two { char a; char b; };
		template <class u> static two test(...);
		template <class u> static char test(typename u::iterator_category* = 0);

	public:
		//判断该T类型里是否有iterator_category;
		static const bool value = sizeof(test<T>(0)) == sizeof(char); 
	};

	//迭代器实现
	template <class Iterator,bool>
	struct iterator_traits_imp{};

	template <class Iterator>
	struct  iterator_traits_imp<Iterator,true>
	{
		typedef typename Iterator::iterator_category    iterator_category;
		typedef typename Iterator::T                    value_type;
		typedef typename Iterator::Pointer              pointer;
		typedef typename Iterator::Reference            reference;
		typedef typename Iterator::Distance             difference_type;

	};


	//helper判断是否是五种迭代器之一
	template <class Iterator,bool>
	struct iterator_traits_helper{};


	template<class Iterator>
	struct iterator_traits_helper<Iterator, true>
		:public iterator_traits_imp<Iterator,
		std::is_convertible<typename Iterator :: iterator_category, input_iterator_tag>::value||
		std::is_convertible<typename Iterator :: iterator_category, output_iterator_tag>::value>
	{
	};

	//开始萃取
	template <class Iterator>
	struct iterator_traits
		:public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

	//特化版
	template <class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag       iterator_category;
		typedef T                                value_type;
		typedef T*                               pointer;
		typedef T&                               reference;
		typedef ptrdiff_t                        difference_type;

	};

	template <class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag       iterator_category;
		typedef T                                value_type;
		typedef const T*                         pointer;
		typedef const T&                         reference;
		typedef ptrdiff_t                        difference_type;

	};

	//真正有大用的萃取机
	template<class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of
		:public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value>
	{};

	//上方萃取机的泛型默认
	template <class T,class U>
	struct has_iterator_cat_of<T, U, false> : public m_false_type {};

	// 萃取迭代器
	template <class Iter>
	struct is_input_iterator :public has_iterator_cat_of<Iter,input_iterator_tag>{};

	template <class Iter>
	struct is_output_iterator :public has_iterator_cat_of<Iter, output_iterator_tag> {};

	template <class Iter>
	struct is_forward_iterator :public has_iterator_cat_of<Iter, forward_iterator_tag> {};

	template <class Iter>
	struct is_bidirectional_iterator:public has_iterator_cat_of<Iter, bidirectional_iterator_tag>{};

	template <class Iter>
	struct is_random_access_iterator :public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

	
	template <class Iterator>
	struct is_iterator :
		public m_bool_constant<is_input_iterator<Iterator> ::value ||
		is_output_iterator<iterator> ::value>
	{
	};

	//萃取迭代器的某些性质
	template<class Iterator>
	typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return Category();
	}

	template<class Iterator>
	typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	template<class Iterator>
	typename iterator_traits<Iterator>::value_type*
		distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	/*******萃取实现示例---计算迭代器之间距离*******/

	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;

		while (first != last)
		{
			++first;
			++n;
		}

		return n;
	}

	//特化版本
	template <class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		distance_dispatch(RandomIterator first, RandomIterator last, random_access_iterator_tag)
	{
		return last - first;
	}

	//对外接口
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last)
	{
		return distance_dispatch(first, last, iterator_category(first));
	}

	/**********迭代器前进n位*************/

	template <class InputIterator,class distance>
	void advance_dispatch(InputIterator& i, distance n, input_iterator_tag)
	{
		while (n--)
			i++;
	}

	template <class BidirectionalIterator,class distance>
	void advance_dispatch(BidirectionalIterator& i, distance n, bidirectional_iterator_tag)
	{
		if (n >= 0)
			while (n--)  ++i;
		else
			while (n++)  --i;
	}

	template <class RandomIterator, class distance>
	void advance_dispatch(RandomIterator& i, distance n, random_access_iterator_tag)
	{
		i += n;
	}

	//对外接口
	template<class InputIter, class distance>
	void advance(InputIter& i,distance n)
	{
		advance_dispatch(i, n, iterator_category(i));
	}

	/********************reverse迭代器实现**********************/

	//反向迭代器，前进为后退，后退为前进

	template<class Iterator>
	class reverse_iterator
	{
	private :
		Iterator current; //记录当前迭代器
	public:
		//设置各类通用名称
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<Iterator>::value_type        value_type;
		typedef typename iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<Iterator>::pointer           pointer;
		typedef typename iterator_traits<Iterator>::reference         reference;

		typedef Iterator                                              iterator_type; //正向迭代器
		typedef reverse_iterator<Iterator>                            self;          //逆向迭代器

	public:
		//构造函数，拷贝构造
		reverse_iterator() {};
		explicit reverese_iterator(iterator_type i) : current(i) {};
		reverse_iterator(const self& rhs) : current(rhs.current) {};

	public:
		//重载基本
		iterator_type base()const
		{
			return current; // 返回正向迭代器
		}
		
		reference operator *() const
		{
			auto tmp = current;
			return *--tmp;
		}

		pointer operator ->()const
		{
			return &(operator *());
		}
		//具体实现

		self& operator++()
		{
			--current;
			return *this;
		}

		self operator++(int)
		{
			self tmp = *this;
			--current;
			return tmp;
		}


		self& operator--()
		{
			++current;
			return *this;
		}



		self operator-(int)
		{
			self tmp = *this;
			++current;
			return tmp;
		}

		self& operator+= (difference_type n)
		{
			current -= n;
			return *this;
		}

		self operator+(difference_type n) const
		{
			return self(current - n);
		}

		self& operator -=(difference_type n)
		{
			current += n;
			return *this;
		}

		self operator -(difference_type n) const
		{
			return self(current + n);
		}

		reference operator[](difference_type n) const
		{
			return *(*this + n);
		}
	};


	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type 
		operator-(const reverse_iterator<Iterator>& lhs, 
			      const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() - lhs.base();
	}

	template <class Iterator>
	bool operator ==(const reverse_iterator<Iterator>& lhs,
		             const reverse_iterator<Iterator>& rhs)
	{
		return lhs.base() == rhs.base();
	}

	template <class Iterator>
	bool operator < (const reverse_iterator<Iterator>& lhs,
		             const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() < lhs.base();
	}

	template <class Iterator>
	bool operator !=(const reverse_iterator<Iterator>& lhs,
		             const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Iterator>
	bool operator >(const reverse_iterator<Iterator>& lhs,
		            const reverse_iterator<Iterator>& rhs)
	{
		return rhs < lhs;
	}

	template <class Iterator>
	bool operator >=(const reverse_iterator<Iterator>& lhs,
		             const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class Iterator>
	bool operator <=(const reverse_iterator<Iterator>& lhs,
		             const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs > rhs);
	}

}


#endif // ! MY_ITERATOR_H_
