//#include <boost/timer/timer.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;
//using namespace boost::timer;

/*
vector<string> createVector_98()
{
    vector<string> vec;
    for (int i = 0; i < 10; ++i){
            vec.emplace_back("helloworld");
    }
    return vec;
}

vector<string> createVector_11()
{
    vector<string> vec;
    for (int i = 0; i < 100; ++i){
        vec.emplace_back("helloworld");
    }
    return move(vec);
}
*/
int main()
{
	/*
    const int TEST_TIMES = 100;

    vector<string> result;

    cpu_timer timer;
    timer.start();
    for (int i = 0; i < TEST_TIMES; ++i){
        result = createVector_98();
    }
    cout << "no move" << timer.format(6) << endl;

    timer.start(); // don't call resume()
    
    for (int i = 0; i < TEST_TIMES; ++i){
        result = createVector_98();
    }
    cout << "use move" << timer.format(6) << endl;

	*/
	return 0;
} 