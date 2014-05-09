#ifndef ACCUM_HPP
#define ACCUM_HPP

#include "accum2.h"
#include "sumpolicy1.h"


/*
template <typename T>
inline typename AccumulationTraits<T>::AccT accum(T const* beg, T const* end)
{
	typedef typename AccumulationTraits<T>::AccT AccT;

	AccT total = AccumulationTraits<T>::zero();

	while (beg != end)
	{
		total += *beg;
		++beg;
	}
	return total;
}
*/


template <typename T, typename Policy = SumPolicy, typename AT = AccumulationTraits<T> >
class Accum
{
public:
	static typename AT::AccT accum(T const* beg, T const* end)
	{
		typename AT::AccT total = AT::zero();
		while (beg != end)
		{
			Policy::accumulate(total, *beg);
			++beg;
		}
		return total;
	}
};

template <typename T>
inline typename AccumulationTraits<T>::AccT accum(T const* beg, T const* end)
{
	return Accum<T>::accum(beg, end);
}


template <typename Traits, typename T>
inline typename Traits::Acct accum(T const* beg, T const* end)
{
	return Accum<T, Traits>::accum(beg, end);
}

#endif