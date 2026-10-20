#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

// Структура сповіщення вхідної пошти
struct Notification
{
    int id;
    std::string category; // категорія (Security, Billing, System, Info)
    int priority;         // пріоритет (від 1 до 3, 1 — найвищий)
};

// звичайна чергу для збереження, але споживачі будуть сортувати виведення
std::queue<Notification> notificationsQueue;
std::mutex queueMutex;
std::mutex outputMutex;
std::condition_variable condition;
bool finished = false;
const int REQUEST_COUNT = 12;

// Потік-виробник: 
void producer()
{
    std::string categories[] = { "Info", "System", "Billing", "Security" };

    for (int i = 1; i <= REQUEST_COUNT; ++i)
    {
        //випадкові параметри 
        int priority = (i % 3) + 1;
        std::string category = categories[(i - 1) % 4];

        Notification notif{ i, category, priority };

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            notificationsQueue.push(notif);
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Producer] Generated Notification ID: " << notif.id
                << ", Category: " << notif.category
                << ", Priority: " << notif.priority << '\n';
        }

        condition.notify_one();           //4 самостійне завдання 
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //затримка відправки
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        finished = true;
    }
    condition.notify_all();

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Producer] Generation finished\n";
    }
}

// потік-споживач: отримує та сортує/обробляє сповіщення за пріоритетом
void consumer(int consumerId)
{
    while (true)
    {
        std::vector<Notification> localBatch;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, []() {
                return !notificationsQueue.empty() || finished;
                });

            if (notificationsQueue.empty() && finished)
            {
                break;
            }

            // Збір елементів з черги для обробки 
            while (!notificationsQueue.empty())
            {
                localBatch.push_back(notificationsQueue.front());
                notificationsQueue.pop();
            }
        }

        //імітація сортування сповіщень за пріоритетом поза критичною секцією
        std::sort(localBatch.begin(), localBatch.end(), [](const Notification& a, const Notification& b) {
            return a.priority < b.priority; 
            });

        //імітація обробки та виведення відсортованих даних
        for (const auto& notif : localBatch)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(notif.priority * 100)); // час обробки 

            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Consumer " << consumerId << "] Sorted & Processed -> ID: " << notif.id
                << ", Category: " << notif.category
                << ", Priority: " << notif.priority << '\n';
        }
    }

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Consumer " << consumerId << "] Finished\n";
    }
}

int main()
{
    // 2 споживачі
    std::thread producerThread(producer);
    std::thread consumer1(consumer, 1);
    std::thread consumer2(consumer, 2);

    producerThread.join();
    consumer1.join();
    consumer2.join();

    std::cout << "\nAll notifications have been sorted and processed.\n";
    return 0;
}