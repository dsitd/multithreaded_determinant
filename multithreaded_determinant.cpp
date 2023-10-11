#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

double get_minor(const std::vector<std::vector<double>>& matrix, int row, int col) {
    std::vector<std::vector<double>> new_matrix(matrix.size() - 1, std::vector<double>(matrix.size() - 1));
    int n = static_cast<int>(matrix.size());
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1; j++) {
            int newRow = (i < row) ? i : i + 1;
            int newCol = (j < col) ? j : j + 1;
            new_matrix[i][j] = matrix[newRow][newCol];
        }
    }
    double determinant = 0.0;
    if (new_matrix.size() == 1) {
        determinant = new_matrix[0][0];
    }
    else {
        for (int j = 0; j < new_matrix.size(); j++) {
            determinant += ((j % 2 == 0) ? 1.0 : -1.0) * new_matrix[0][j] * get_minor(new_matrix, 0, j);
        }
    }
    return determinant;
}

double get_determinant(const std::vector<std::vector<double>>& matrix, int num_threads) {
    double determinant = 0.0;
    int n = static_cast<int>(matrix.size());

    std::vector<std::thread> threads(num_threads);
    std::vector<double> results(num_threads, 0.0);

    for (int i = 0; i < num_threads; i++) {
        threads[i] = std::thread([=, &results]() {
            int start = i * n / num_threads;
            int end = (i + 1) * n / num_threads;
            for (int j = start; j < end; j++) {
                results[i] += ((j % 2 == 0) ? 1.0 : -1.0) * matrix[0][j] * get_minor(matrix, 0, j);
            }
            });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    for (int i = 0; i < num_threads; i++) {
        determinant += results[i];
    }

    return determinant;
}

int main() {
    std::vector<std::vector<double>> matrix = {
        {6, 8, -1, -10},
        {-4, 9, 4, -8},
        {5, 8, 10, 2},
        {0, 7, 3, -2}
    };

    int num_threads = 5;
    double determinant = 0.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    determinant = get_determinant(matrix, num_threads);
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    std::cout << "Number of threads: " << num_threads << std::endl;
    std::cout << "Determinant: " << determinant << std::endl;
    std::cout << "Execution time: " << duration << " microseconds" << std::endl;

    return 0;
}
