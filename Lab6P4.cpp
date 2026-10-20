#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>

// Послідовне обчислення суми членів ряду для e^x
double computeExponentialSeriesSequential(double x, int total_terms)
{
    double sum = 0.0;
    double term = 1.0; // Початковий член ряду для n = 0 (x^0 / 0! = 1)

    for (int n = 0; n <= total_terms; ++n)
    {
        if (n > 0)
        {
            term *= x / n; //наступний член: term_n = term_{n-1} * (x / n)
        }
        sum += term;
    }
    return sum;
}

int main()
{
    std::cout << std::fixed << std::setprecision(12);

    double x = 2.5;        
    int total_terms = 30;   // Кількість членів ряду

    std::cout << "Sequential Algorithm\n";
    std::cout << "Target: e^" << x << " with " << total_terms << " terms\n\n";

    //час виконання
    auto start = std::chrono::high_resolution_clock::now();

    double seq_result = computeExponentialSeriesSequential(x, total_terms);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    //порівняння зі значенням std::exp
    double exact_value = std::exp(x);

    std::cout << "Sequential Result : " << seq_result << '\n';
    std::cout << "Exact std::exp(x) : " << exact_value << '\n';
    std::cout << "Execution Time    : " << duration.count() << " ms\n";

    return 0;
}