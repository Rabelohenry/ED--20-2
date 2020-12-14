#include <iostream>

using namespace std;

bool prime(int a);

void fatoration(int a);

int main()
{
  int a;

  cin >> a;

  fatoration(a);

  return 0;
}

bool prime(int a)
{
  int prime = 0;

  for (int i = 1; i <= a; i++)
  {
    if (a % i == 0)
    {
      prime++;
    }
  }

  return (prime == 2);
}

void fatoration(int a)
{
  for (int i = 1; i < a; i++)
  {
    for (int j = 1; j < a; j++)
    {
      if (i * j == a)
      {
        if ((prime(i) == true) and (prime(j) == true))
        {
          cout << i << " " << j << endl;
        }
      }
    }
  }
}