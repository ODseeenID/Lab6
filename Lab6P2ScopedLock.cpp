#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>

std::mutex mutexA;
std::mutex mutexB;
std::barrier syncPoint(2); // бар'єр для синхронізації двох потоків 

void task1()
{
    syncPoint.arrive_and_wait(); //синхронізуємо старт
    std::scoped_lock lock(mutexA, mutexB); // одночасне захоплення обох м'ютексів
    std::cout << "Thread 1: both mutexes locked\n";
}

void task2()
{
    syncPoint.arrive_and_wait(); //синхронізуємо старт
    std::scoped_lock lock(mutexB, mutexA); // одночасне захоплення обох м'ютексів
    std::cout << "Thread 2: both mutexes locked\n";
}

int main()
{
    std::thread t1(task1);
    std::thread t2(task2);

    t1.join();
    t2.join();

    std::cout << "Program finished\n";
    return 0;
}