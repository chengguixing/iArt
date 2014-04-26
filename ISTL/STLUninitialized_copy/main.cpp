// uninitialized_copy example
#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>

int main () {
	std::string numbers[] = {"one", "two", "three"};

	// get block of uninitialized memory:
	std::pair <std::string*, std::ptrdiff_t> result = std::get_temporary_buffer<std::string>(3);

	if (result.second > 0) 
	{
		std::uninitialized_copy ( numbers, numbers + result.second, result.first );

		for (int i = 0; i < result.second; ++i)
		{
			std::cout << result.first[i] << " ";
		}
		std::cout << '\n';
		std::return_temporary_buffer(result.first);
	}

	system("pause");
	return 0;
}