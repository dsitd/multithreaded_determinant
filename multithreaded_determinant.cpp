#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

std::vector<std::vector<std::vector<double>>> generateMinors(const std::vector<std::vector<double>>& matrix) {
    size_t n = matrix.size();
    std::vector<std::vector<std::vector<double>>> minors;
    for (size_t col = 0; col < n; ++col) {
        std::vector<std::vector<double>> minorMatrix(n - 1, std::vector<double>(n - 1));
        for (size_t i = 1; i < n; ++i) {
            size_t minorCol = 0;
            for (size_t j = 0; j < n; ++j) {
                if (j != col) {
                    minorMatrix[i - 1][minorCol] = matrix[i][j];
                    minorCol++;
                }
            }
        }
        minors.push_back(minorMatrix);
    }
    return minors;
}

double determinant(const std::vector<std::vector<double>>& matrix, std::mutex& mtx, int& activeThreads, const int maxThreads) {
    size_t n = matrix.size();
    if (n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    double det = 0.0;
    std::vector<std::thread> threads;

    auto minors = generateMinors(matrix);

    for (size_t col = 0; col < n; ++col) {
        if (activeThreads < maxThreads) {
            std::lock_guard<std::mutex> lock(mtx);
            activeThreads++;
            threads.emplace_back([&, col]() {
                double minorValue = determinant(minors[col], mtx, activeThreads, maxThreads);

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (col % 2 == 0)
                        det += matrix[0][col] * minorValue;
                    else
                        det -= matrix[0][col] * minorValue;

                    activeThreads--;
                }
                });
        }
        else {
            double minorValue = determinant(minors[col], mtx, activeThreads, maxThreads);
            if (col % 2 == 0)
                det += matrix[0][col] * minorValue;
            else
                det -= matrix[0][col] * minorValue;
        }
    }
    
    for (auto& t : threads) {
        t.join();
    }

    return det;
}

int main() {
    std::vector<std::vector<double>> matrix = {
        {10, 10, 7, 8, 7, 5},
        {8, 3, 6, 9, 1, 4},
        {5, 7, 3, 6, 4, 6},
        {2, 1, 10, 3, 1, 4},
        {8, 3, 1, 2, 5, 4},
        {6, 1, 4, 1, 3, 4}
    };

    std::mutex mtx;
    int activeThreads = 0;
    const int maxThreads = 10;

    double det = determinant(matrix, mtx, activeThreads, maxThreads);
    std::cout << "Determinant: " << det << std::endl;

    return 0;
}
