#ifndef POW3_HPP
#define POW3_HPP


template <int N>
class Pow3
{
public:
	//enum{result = 3 * Pow3<N - 1>::result};
	static int const result = 3 * Pow3<N - 1>::result;
};

template <>
class Pow3<0>
{
public:
	//enum{result = 1};
	static int const result = 1;
};


#endif