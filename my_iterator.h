#ifndef  MY_ITERATOR_H_
#define  MY_ITERATOR_H_

#include<cstddef>
#include"my_type_traits.h"


namespace mystl
{
	//���ֵ�����
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag{};
	struct bidirectional_iterator_tag : public forward_iterator_tag{};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	//������ģ��

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
		//�жϸ�T�������Ƿ���iterator_category;
		static const bool value = sizeof(test<T>(0)) == sizeof(char); 
	};

	//������ʵ��
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


	//helper�ж��Ƿ������ֵ�����֮һ
	template <class Iterator,bool>
	struct iterator_traits_helper{};


	template<class Iterator>
	struct iterator_traits_helper<Iterator, true>
		:public iterator_traits_imp<Iterator,
		std::is_convertible<typename Iterator :: iterator_category, input_iterator_tag>::value||
		std::is_convertible<typename Iterator :: iterator_category, output_iterator_tag>::value>
	{
	};

	//��ʼ��ȡ
	template <class Iterator>
	struct iterator_traits
		:public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

	//�ػ���
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

	//�����д��õ���ȡ��
	template<class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of
		:public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value>
	{};

	//�Ϸ���ȡ���ķ���Ĭ��
	template <class T,class U>
	struct has_iterator_cat_of<T, U, false> : public m_false_type {};

	// ��ȡ������
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

	//��ȡ��������ĳЩ����
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

	/*******��ȡʵ��ʾ��---���������֮�����*******/

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

	//�ػ��汾
	template <class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		distance_dispatch(RandomIterator first, RandomIterator last, random_access_iterator_tag)
	{
		return last - first;
	}

	//����ӿ�
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last)
	{
		return distance_dispatch(first, last, iterator_category(first));
	}

	/**********������ǰ��nλ*************/

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

	//����ӿ�
	template<class InputIter, class distance>
	void advance(InputIter& i,distance n)
	{
		advance_dispatch(i, n, iterator_category(i));
	}

	/********************reverse������ʵ��**********************/

	//�����������ǰ��Ϊ���ˣ�����Ϊǰ��

	template<class Iterator>
	class reverse_iterator
	{
	private :
		Iterator current; //��¼��ǰ������
	public:
		//���ø���ͨ������
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<Iterator>::value_type        value_type;
		typedef typename iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<Iterator>::pointer           pointer;
		typedef typename iterator_traits<Iterator>::reference         reference;

		typedef Iterator                                              iterator_type; //���������
		typedef reverse_iterator<Iterator>                            self;          //���������

	public:
		//���캯������������
		reverse_iterator() {};
		explicit reverese_iterator(iterator_type i) : current(i) {};
		reverse_iterator(const self& rhs) : current(rhs.current) {};

	public:
		//���ػ���
		iterator_type base()const
		{
			return current; // �������������
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
		//����ʵ��

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
