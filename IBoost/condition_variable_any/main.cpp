#include <windows.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;
#include <stack>

mutex io_mu;

class buffer
{
private:
	mutex mu;
	condition_variable_any cond_put;
	condition_variable_any cond_get;
	stack<int> stk;
	int un_read, capacity;
	bool is_full()
	{
		return un_read == capacity;
	}
	bool is_empty()
	{
		return un_read == 0;
	}
public:
	buffer(size_t n) : un_read(0), capacity(n)
	{
		;
	}

	void put(int x)
	{
		{
				mutex::scoped_lock lock(io_mu);
				cout << "enter put" << endl;
		}

		{
			mutex::scoped_lock lock(mu);
			while (is_full())
			{
				{
					mutex::scoped_lock lock(io_mu);
					cout << "enter while (is_full())" << endl;
				}
				{
					mutex::scoped_lock lock(io_mu);
					cout << "full waiting..." << endl;
				}
				cond_put.wait(mu);
				{
					mutex::scoped_lock lock(io_mu);
					cout << "leave while (is_full())" << endl;
				}
			}
			stk.push(x);
			++un_read;
		}
		cond_get.notify_one();


		{
				mutex::scoped_lock lock(io_mu);
				cout << "leave put" << endl;
		}
	}

	void get(int *x)
	{
		{
				mutex::scoped_lock lock(io_mu);
				cout << "enter get" << endl;
		}
		{
			mutex::scoped_lock lock(mu);
			while (is_empty())
			{
				{
				mutex::scoped_lock lock(io_mu);
				cout << "enter while (is_empty())" << endl;
				}
				{
					mutex::scoped_lock lock(io_mu);
					cout << "empty waiting ..." << endl;
				} 
				cond_get.wait(mu);
				{
				mutex::scoped_lock lock(io_mu);
				cout << "leave while (is_empty())" << endl;
				}
			}
			--un_read;
			*x = stk.top();
			stk.pop();
		}
		cond_put.notify_one();
		
		{
				mutex::scoped_lock lock(io_mu);
				cout << "leave get" << endl;
		}
	}
};



buffer buf(5);

void producer(int n)
{
	for (int i = 0; i < n; ++i)
	{
		{
			mutex::scoped_lock lock(io_mu);
			cout << "put " << i << endl;
		}
		buf.put(i);
	}
}


void consumer(int n)
{
	int x;
	for (int i = 0; i < n; ++i)
	{
		buf.get(&x);
		mutex::scoped_lock lock(io_mu);
		cout << "get " << x << endl;
	}
}



int main() 
{ 
	thread t1(producer, 20);
	thread t2(consumer, 10);
	thread t3(consumer, 10);

	t1.join();
	t2.join();
	t3.join();


	getchar();
	return 0;
} 