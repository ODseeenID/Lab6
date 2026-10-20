#include <iostream>
#include <future>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>

//послідовне обчислення суми членів ряду для e^x (або окремого діапазону для паралельної версії)
double computeSeriesRange(double x, int start_n, int end_n)
{
    double sum = 0.0;
    double term = 1.0; //початковий член ряду для n = 0 (x^0 / 0! = 1)

    // Якщо діапазон починається не з 0, обчислюємо початковий член для start_n
    for (int i = 1; i <= start_n; ++i)
    {
        term *= x / i;
    }

    for (int n = start_n; n <= end_n; ++n)
    {
        if (n > 0 && n > start_n)
        {
            term *= x / n; // наступний член: term_n = term_{n-1} * (x / n)
        }
        else if (n == 0 && start_n == 0)
        {
            term = 1.0;
        }
        sum += term;
    }
    return sum;
}

//послідовна функція для повного діапазону
double computeExponentialSeriesSequential(double x, int total_terms)
{
    return computeSeriesRange(x, 0, total_terms);
}

int main()
{
    std::cout << std::fixed << std::setprecision(12);

    double x = 2.5;
    int total_terms = 1000;   //кількість членів ряду (для експериментів з обсягами даних) 
    int num_tasks = 16;      //кількість асинхронних задач 

    std::cout << "Parallel Algorithm with std::async / std::future \n";
    std::cout << "Target: e^" << x << " with " << total_terms << " terms, Tasks: " << num_tasks << "\n\n";

    //послідовне виконання 
    auto start_seq = std::chrono::high_resolution_clock::now();

    double seq_result = computeExponentialSeriesSequential(x, total_terms);

    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> seq_duration = end_seq - start_seq;


    //паралельне виконання
    auto start_par = std::chrono::high_resolution_clock::now();

    int chunk_size = (total_terms + 1) / num_tasks;
    std::vector<std::future<double>> futures;

    for (int i = 0; i < num_tasks; ++i)
    {
        int start_n = i * chunk_size;
        int end_n = (i == num_tasks - 1) ? total_terms : (start_n + chunk_size - 1);

        //запуск асинхронної задачі у новому потоці
        futures.push_back(std::async(std::launch::async, computeSeriesRange, x, start_n, end_n));
    }

    // збір результатів через .get() та об'єднання часткових сум
    double par_result = 0.0;
    for (auto& f : futures)
    {
        par_result += f.get();
    }

    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> par_duration = end_par - start_par;


    // перевірка результатів, розрахунок прискорення
    double exact_value = std::exp(x);
    double diff = std::abs(seq_result - par_result);
    double speedup = seq_duration.count() / par_duration.count();

    std::cout << "Sequential Result   : " << seq_result << " (Time: " << seq_duration.count() << " ms)\n";
    std::cout << "Parallel Result     : " << par_result << " (Time: " << par_duration.count() << " ms)\n";
    std::cout << "Exact std::exp(x)   : " << exact_value << '\n';
    std::cout << "Absolute difference : " << diff << "\n";
    std::cout << "Speedup (S)         : " << speedup << "x\n";

    return 0;
}