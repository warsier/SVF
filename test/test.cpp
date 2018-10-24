#include <iostream>

using namespace std;

int func_first(int zeta, int gamma)
{
	return (zeta * gamma);
}

int func_second(int gamma)
{
	return (gamma + 2);
}

int main()
{
	int number = 5;
	int zeta = 0;
	int gamma = 0;
	int *ptr = &zeta;
	int iter = 1;
	while (iter < number) {
		if (iter < 3)
			zeta = func_first(*ptr, gamma);
		else 
			zeta = func_first(iter, gamma);
		gamma = func_second(zeta);
		iter++;
	}

	return gamma;
}
