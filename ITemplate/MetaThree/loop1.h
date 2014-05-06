#ifndef LOOP1_HPP
#define LOOP1_HPP

template <typename T>
inline T dot_product(int dim, T* a, T* b)
{
	T result = T();
	for (int i = 0; i < dim; ++i)
	{
		result += a[i] * b[i];
	}
	return result;
}

#endif