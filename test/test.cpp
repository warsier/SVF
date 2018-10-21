#include <iostream>

using namespace std;

int f1(int z, int y)
{
	return (z * y);
}

int f2(int y)
{
	return (y + 2);
}

int main()
{
	int n;
	cin >> n;
	int z = 0;
	int y = 0;
	int i = 1;
	while (i < n) {
		z = f1(z, y);
		y = f2(y);
		i++;
	}
	cout << z;
	return 0;
}
