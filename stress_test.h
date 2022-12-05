#include <vector>
#include <random>
#include <algorithm>

using std::vector;

std::mt19937 GENERATOR;

void set_seed(size_t seed) {
    GENERATOR = std::mt19937(seed);
}

template <class  T>
T from_range(const T min, const T max) {
    return T(GENERATOR()%(max - min + 1)) + min;
}

template <class  T>
void fill_random(vector<T>* arr, const int min, const int max) {
    for (auto& elem: *arr) {
        elem = from_range<T>(min, max);
    }
}

template <class  T>
vector<T> generate_vector(const size_t size, const int min, const int max) {
    vector<T> arr(size);
    fill_random<T>(&arr, min, max);
    return arr;
}

template <class  T>
vector<T> sorted(vector<T> arr) {
    std::sort(arr.begin(), arr.end());
    return arr;
}

vector<size_t> range(size_t from, size_t to) {
    vector<size_t> ret;
    for (size_t i = from; i < to; ++i)
        ret.push_back(i);
    return ret;
}

vector<size_t> range(size_t to) {
    return range(0, to);
}
