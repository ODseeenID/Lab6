#include <iostream>
#include <thread>
#include <mutex>
#include <chrono> // вимірювання часу

int counter = 0;
const int ITERATIONS = 16 * 100000;
std::mutex counterMutex; // м'ютекс

void increment()
{
    for (int i = 0; i < ITERATIONS; ++i)
    {
        // RAII-об'єкт lock_guard автоматично захоплює м'ютекс і звільняє його в кінці блоку
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter;
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