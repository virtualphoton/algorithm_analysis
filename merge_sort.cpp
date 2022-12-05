#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "stress_test.h"

using std::vector;

// merge sort looks like this, because it's taken from my code review
size_t miscellaneous, copies, comparisons;

template <class RandomAccessIteratorInput, class RandomAccessIteratorOutput,
          class CompareFunction>
void Merge(RandomAccessIteratorInput first_begin,
           RandomAccessIteratorInput first_end,
           RandomAccessIteratorInput second_begin,
           RandomAccessIteratorInput second_end,
           RandomAccessIteratorOutput output,
           CompareFunction comp) {
            
    while (first_begin != first_end && second_begin != second_end) {
        if (comp(*second_begin, *first_begin)) {
            *output = std::move(*second_begin);
            ++second_begin;
        } else {
            // goes here, if first <= second, making the sort stable
            *output = std::move(*first_begin);
            ++first_begin;
        }
        ++output;
        
        ++copies;
    }
    
    copies += std::distance(first_begin, first_end);
    copies += std::distance(second_begin, second_end);
    std::move(first_begin, first_end, output);
    std::move(second_begin, second_end, output);
}
 

template <class RandomAccessIterator>
inline RandomAccessIterator AddToIteratorSafe(const RandomAccessIterator& begin,
                                              const RandomAccessIterator& end,
                                              const size_t value) {
    // distance(begin, end) expected to be >= 0
    return value > size_t(std::distance(begin, end))? end : begin + value;
}


template <class RandomAccessIteratorInput, class RandomAccessIteratorOutput,
          class CompareFunction>
inline void MergeChunks(const RandomAccessIteratorInput& input_begin,
                        const RandomAccessIteratorInput& input_end,
                        const RandomAccessIteratorOutput& output_begin,
                        CompareFunction comp,
                        const size_t chunk_size) {
    Merge(
        input_begin,
        AddToIteratorSafe(input_begin, input_end, chunk_size),
        AddToIteratorSafe(input_begin, input_end, chunk_size),
        AddToIteratorSafe(input_begin, input_end, 2*chunk_size),
        output_begin,
        comp
    );
}


template <class RandomAccessIterator, class CompareFunction>
void Sort(const RandomAccessIterator first,
          const RandomAccessIterator last,
          CompareFunction comp) {
    
    const size_t size = std::distance(first, last);
    vector<typename RandomAccessIterator::value_type> buffer(size);
    bool buffer_is_output = true;
    
    for (size_t chunk_size = 1; chunk_size < size; chunk_size *= 2) {
        auto source_begin = first;
        auto buffer_begin = buffer.begin();
        
        while (source_begin != last) {
            if (buffer_is_output) {
                MergeChunks(source_begin, last, buffer_begin, comp, chunk_size);
            } else {
                MergeChunks(buffer_begin, buffer.end(), source_begin, comp, chunk_size);
            }
            
            source_begin = AddToIteratorSafe(source_begin, last, 2*chunk_size);
            buffer_begin = AddToIteratorSafe(buffer_begin, buffer.end(), 2*chunk_size);
            ++miscellaneous;
        }
        
        buffer_is_output = !buffer_is_output;
    }
    
    if (!buffer_is_output) {
        copies += std::distance(buffer.begin(), buffer.end());
        std::move(buffer.begin(), buffer.end(), first);
    }
}

class Less {
    std::less<int> comp;
public:
    bool operator()(const int lhs, const int rhs) {
        ++comparisons;
        return comp(lhs, rhs);
    }
};

void PrintArr(std::string name, const vector<int>& arr, std::ofstream& output) {
    output << "\"" << name << "\":[";
    for (auto elem : arr) {
        output << elem << ",";
    }
    output << "],";
}

void Test(const size_t length, const size_t n_samples, std::ofstream& output, const size_t seed) {
    set_seed(seed);
    Less less;
    output << "{\"length\":" << length << ",";
    vector<int> misc_vec, cp_vec, comp_vec;
    for (auto i : range(n_samples)) {
        miscellaneous = copies = comparisons = 0;
        vector<int> arr(10);
        auto random_nums = generate_vector<int>(length, -500'000'000, 500'000'000);
        auto random_nums_sorted = sorted(random_nums);
        Sort(random_nums.begin(), random_nums.end(), less);
        if (random_nums != random_nums_sorted) {
            std::cout << length << seed << i << std::endl;
            throw "sorting procedure is incorrect!";
        }
        misc_vec.push_back(miscellaneous);
        cp_vec.push_back(copies);
        comp_vec.push_back(comparisons);
    }
    PrintArr("misc", misc_vec, output);
    PrintArr("copies", cp_vec, output);
    PrintArr("comparisons", comp_vec, output);
    output << "},";
}

int main() {
    std::ofstream output("output.txt");
    output << "[";
    for (auto length : range(10, 2000)) {
        Test(length, 50, output, time(nullptr) + length);
    }
    output << "]";
    output.close();
    return 0;
}
