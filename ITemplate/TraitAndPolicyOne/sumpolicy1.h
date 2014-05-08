#ifndef SUMPOLICY1_HPP
#define SUMPOLICY1_HPP



class SumPolicy
{
public:
	template <typename T1, typename T2>
	static void accumulate(T1& total, T2 const& value)
	{
		total += value;
	}
};
#endif