#include <iostream>

using namespace std;

int reverse(int a, int b);

int main() {
  int a, b;

  cin >> a >> b;

  cout << reverse(a, b) << endl;

  return 0;
}

int reverse(int a, int b) {
  for (int i = 1; i < b; i++) {
    if (((a * i) % b) == 1) {
      return i;
    }
  }
}