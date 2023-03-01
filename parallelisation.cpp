#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <chrono>

const int NUM_THREADS = std::thread::hardware_concurrency();
const int MAX = 1000000;
std::mutex mtx;
std::vector<int> data(MAX);

void generate_data(int start, int end) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 19);
    for (int i = start; i < end; ++i) {
        data[i] = dis(gen);
    }
}

void compute_sum(int start, int end, long long& sum) {
    sum = 0;
    for (int i = start; i < end; ++i) {
        sum += data[i];
    }
}

int main() {
    std::vector<std::thread> threads(NUM_THREADS);
    std::vector<long long> sums(NUM_THREADS);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Generate data using multiple threads
    int range = MAX / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; ++i) {
        int start = i * range;
        int end = start + range;
        threads[i] = std::thread(generate_data, start, end);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    // Compute sum using multiple threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        int start = i * range;
        int end = start + range;
        threads[i] = std::thread(compute_sum, start, end, std::ref(sums[i]));
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    // Update global sum
    long long global_sum = 0;
    for (int i = 0; i < NUM_THREADS; ++i) {
        global_sum += sums[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << "Final sum: " << global_sum << std::endl;
    std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms" << std::endl;

    return 0;
}
