#pragma once
#ifndef __UNINITIALIZED_H__
#define __UNINITIALIZED_H__

#include <memory>
#include "Iterator.h"
#include "TypeTraits.h"

//uninitialized_fill_n()
//迭代器first指向欲初始化的空间的起始处，n表示欲初始化空间的大小，x表示初值
template <class ForwardIterator,class Size,class T>
inline ForwardIterator UninitializedFillN(ForwardIterator first, Size n, const T& x)
{
	return __UninitializedFillN(first, n, x, ValueType(first));
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __UninitializedFillN(ForwardIterator first, Size n, const T& x, T*)
{
	typedef typename __TypeTraits<T>::IsPODType IsPODType;
	return __UninitializedFillNAux(first, n, x, IsPODType());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __UninitializedFillNAux(ForwardIterator first, Size n, const T& x, __TrueType)
{
	// 这里实际是调用的C++库里的fill()
	return fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __UninitializedFillNAux(ForwardIterator first, Size n, const T& x, __FalseType)
{
	ForwardIterator cur = first;
	try{
		for (; n > 0; --n, ++cur)
		{
			Construct(&(*cur), x);//取到对象的地址
		}
	}
	catch (...)
	{
		Destroy(first, cur);
		throw;
	}
	return cur;
}


//uninitialized_copy()
//迭代器first指向输入端的起始位置，迭代器last指向输入端的结束位置(前闭后开)，
//迭代器result指向输出端（欲初始化空间）的起始处
template <class InputIterator,class ForwardIterator>
inline ForwardIterator UninitializedCopy(InputIterator first, InputIterator last, ForwardIterator result)
{
	return __UninitializedCopy(first, last, result, ValueType(result));
}
template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __UninitializedCopy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
	typedef typename __TypeTraits<T>::IsPODType IsPODType;
	return __UninitializedCopyAux(first, last, result, IsPODType());
}
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __UninitializedCopyAux(InputIterator first, InputIterator last, ForwardIterator result, __TrueType)
{
	// 这里实际是调用的C++库里的copy
	// 内部要通过模板推演出数据的类型，再调用memmove/memcopy
	return copy(first, last, result);
}
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __UninitializedCopyAux(InputIterator first, InputIterator last, ForwardIterator result, __FalseType)
{
	ForwardIterator cur = result;
	try{
		for (; first != last; ++first, ++cur)
		{
			Construct(&(*cur), *first);//必须一个一个元素进行，无法批量进行
		}
	}
	catch (...)
	{
		Destory(result, cur);
		throw;
	}
	return cur;
}
/*针对char*和wchar_t*两种型别，可以采用最有效率的做法memmove来执行复制行为*/
inline char* UninitializedCopy(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}
inline wchar_t* UninitializedCopy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, (last - first) * sizeof(wchar_t));
	return result + (last - first);
}


//uninitialized_fill
//迭代器first指向输出端（欲初始化空间）的起始处，迭代器last指向输出端的结束处（前闭后开），x表示初值
template <class ForwardIterator, class T>
inline void UninitializedFill(ForwardIterator first, ForwardIterator last, const T& x)
{
	return __UninitializedFill(first, last, x, ValueType(first));
}
template <class ForwardIterator, class T>
inline void __UninitializedFill(ForwardIterator first, ForwardIterator last, const T& x, T*)
{
	typedef typename __TypeTraits<T>::IsPODType IsPODType;
	return __UninitializedFillAux(first, last, x, IsPODType());
}
template <class ForwardIterator, class T>
inline void __UninitializedFillAux(ForwardIterator first, ForwardIterator last, const T& x, __TrueType)
{
	fill(first, last, x);
}
template <class ForwardIterator, class T>
inline void __UninitializedFillAux(ForwardIterator first, ForwardIterator last, const T& x, __FalseType)
{
	ForwardIterator cur = first;
	try{
		for (; cur != last; ++cur)
		{
			Construct(&(*cur), x);
		}
	}
	catch (...)
	{
		Destory(first, cur);
		throw;
	}
}

#endif /*__UNINITIALIZED_H__*/
