#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

uint64_t area(vector<pair<int, int>> &points) {
  // This code has signed overflows. :)
  // Please fix this so it correctly evaluates area..!
  static int n_max = 100;
  vector<uint64_t> pos, neg; // 0 ~ 2^61
  uint64_t total = 0;
  size_t n = points.size();

  for (unsigned i = 0; i < n; i++) {
    unsigned j = (i + 1) % n;
    int x_i = points[i].first; // -2^31 ~ 2^31 - 1
    int y_i = points[i].second; // -2^31 ~ 2^31 - 1
    int x_j = points[j].first; // -2^31 ~ 2^31 - 1
    int y_j = points[j].second; // -2^31 ~ 2^31 - 1
    int64_t seg1 = (int64_t)x_i * y_j; // -2^62 + 2^31 ~ 2^62
    int64_t seg2 = -(int64_t)x_j * y_i; // -2^62 + 2^31 ~ 2^62
    total += (seg1 & 1) + (seg2 & 1);
    seg1 >>= 1; // -2^61 + 2^30 ~ 2^61
    seg2 >>= 1; // -2^61 + 2^30 ~ 2^61
    seg1 >= 0 ? pos.emplace_back(seg1) : neg.emplace_back(-seg1); // 0 ~ 2^61
    seg2 >= 0 ? pos.emplace_back(seg2) : neg.emplace_back(-seg2); // 0 ~ 2^61
  }
  total >>= 1;
  sort (pos.begin(), pos.end(), greater<uint64_t>());
  sort (neg.begin(), neg.end(), greater<uint64_t>());

  auto it = neg.begin();
  for (uint64_t posnum : pos) {
    // total >= 0 guaranteed
    // if neg remains, it means total < 2^61
    // if there's no neg, just adding is fine, sum <= 2^64 - 2^33 + 1 guaranteed
    total += posnum;
    while (it != neg.end() && (*it) <= total) {
      total -= (*it);
      it++;
    }
  }
  return total;
}
