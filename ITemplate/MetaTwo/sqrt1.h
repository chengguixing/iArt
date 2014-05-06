#ifndef SQRT1_HPP
#define SQRT1_HPP


/*
template <bool C, typename Ta, typename Tb>
class IfThenElse;

template <typename Ta, typename Tb>
class IfThenElse<true, Ta, Tb>
{
public:
	typedef Ta ResultT;
};

template <typename Ta, typename Tb>
class IfThenElse<false, Ta, Tb>
{
public:
	typedef Tb ResultT;
};

template <int N, int LO = 0, int HI = N>
class Sqrt
{
public:
	enum{mid = (LO + HI + 1) / 2};

	typedef typename IfThenElse<(N < mid * mid), Sqrt<N, LO, mid - 1>, Sqrt<N, mid, HI> >::ResultT SubT;

	enum{result = SubT::result};
};

template <int N, int M>
class Sqrt<N, M, M>
{
public:
	enum{result = M};
};
*/


template <bool C, typename Ta, typename Tb>
class IfThenElse;

template <typename Ta, typename Tb>
class IfThenElse<true, Ta, Tb>
{
public:
	typedef Ta ResultT;
};

template <typename Ta, typename Tb>
class IfThenElse<false, Ta, Tb>
{
public:
	typedef Tb ResultT;
};


template <int N>
class Value
{
public:
	enum{result = N};
};

template <int N, int I = 0>
class Sqrt
{
public:
	typedef typename IfThenElse<I * I < N, Sqrt<N, I + 1>, Value<I> >::ResultT SubT;
	enum{result = SubT::result};
};

#endif