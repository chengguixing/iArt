#include <boost/timer.hpp>
#include <boost/static_assert.hpp>
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;


template <int N = 2>
class iProgress_timer : public timer
{
public:
	iProgress_timer(std::ostream& os = std::cout) : m_os(os)
	{
		BOOST_STATIC_ASSERT(N >= 0 && N <= 10);
	}
	~iProgress_timer()
	{
		try
		{
			std::istream::fmtflags old_flags = m_os.setf(std::istream::fixed, std::istream::floatfield);
			std::streamsize old_prec = m_os.precision(N);
			m_os << elapsed() << "s" << std::endl;

			m_os.flags(old_flags);
			m_os.precision(old_prec);
		}
		catch (...)
		{
			;
		}
	}
private:
	std::ostream& m_os;
};

int main() 
{ 
	iProgress_timer<10> t;
	cout << t.elapsed_max() << endl;
	cout << t.elapsed_min() << endl;
	cout << t.elapsed() << endl;

	system("pause");
	return 0;
} 