#include <iostream>
#include <cstring>
using namespace std;
constexpr size_t MAX_N = 16;
int arr[MAX_N];

int main() {
    int n, t, p;
    cin >> n;

    memset(arr, 0, sizeof(arr));

    for(int i = 0; i < n; i++){
      cin >> t >> p;
      arr[i + t] = max(arr[i + t], arr[i] + p);

      for(int j = i; j <= n; j++){
          arr[j] = max(arr[i], arr[j]);
      }

    }

    cout << arr[n];
    return 0;
}