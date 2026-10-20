#include <iostream>
#include <thread>
#include <atomic>
#include <chrono> // вимірювання часу

std::atomic<int> counter{ 0 }; // атомарна змінна
const int ITERATIONS = 16 * 100000;

void increment()
{
    for (int i = 0; i < ITERATIONS; ++i)
    {
        ++counter; // атомарний інкремент
    }
}

int main()
{
    auto start = std::chrono::steady_clock::now();

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    const int expected = 2 * ITERATIONS;
    std::cout << "Expected value: " << expected << '\n';
    std::cout << "Actual value: " << counter << '\n';
    std::cout << "Time elapsed: " << elapsed.count() << " ms\n";

    return 0;
}