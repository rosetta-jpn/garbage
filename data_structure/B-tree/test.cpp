#include <iostream>
#include <set>
#include <vector>
#include "BTree.hpp"
using namespace std;
enum op{
  INSERT,
  CONTAIN,
  ERASE,
  SIZE
};

int main()
{
  BTree<int> tree;
  srand(time(NULL));
  constexpr int Q = 1000000;
  srand(time(NULL));
  vector <int> memo(1000);
  for (int i = 0; i < 1000; i++) {
    memo[i] = i;
  }
  set <int> test;
  for (int q = 0; q < Q; q++) {
    int t = rand() % 4;
    int x = memo[rand() % 1000];
    switch(t){
    case INSERT:
      tree.insert(x);
      test.insert(x);
      break;
    case CONTAIN:
      {
        auto b = tree.contain(x);
        auto b2 = test.find(x) != test.end();
        if (b != b2) {
          cout << "CONTAIN ERROR" << endl;
          cout << b << " " << b2 << " " << x << endl;
          break;
        }
      }
      break;
    case ERASE:
      tree.erase(x);
      test.erase(x);
      break;
    case SIZE:
      {
        auto s = tree.size();
        auto s2 = test.size();
        if (s != s2) {
          cout << "SIZE ERROR" << endl;
          cout << s << " " << s2 << endl;
          break;
        }
      }
      break;
    }
  }
  return 0;
}

