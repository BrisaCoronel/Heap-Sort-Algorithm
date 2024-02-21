#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <mutex>
#include <cmath>

std::mutex mtx;

void heapify(std::vector<int>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(std::vector<int>& arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void parallelHeapSort(std::vector<int>& arr, int n, int depth = 0) {
    if (depth >= log2(std::thread::hardware_concurrency())) {
        heapSort(arr, n);
    }
    else {
        int mid = n / 2;
        std::vector<int> left_arr(arr.begin(), arr.begin() + mid);
        std::vector<int> right_arr(arr.begin() + mid, arr.end());

        std::thread left_thread(parallelHeapSort, std::ref(left_arr), left_arr.size(), depth + 1);
        parallelHeapSort(right_arr, right_arr.size(), depth + 1);

        left_thread.join();

        int i = 0, j = 0, k = 0;
        while (i < left_arr.size() && j < right_arr.size()) {
            if (left_arr[i] < right_arr[j])
                arr[k++] = left_arr[i++];
            else
                arr[k++] = right_arr[j++];
        }
        while (i < left_arr.size())
            arr[k++] = left_arr[i++];
        while (j < right_arr.size())
            arr[k++] = right_arr[j++];
    }
}

int main() {
    auto start_total = std::chrono::steady_clock::now();
    auto start_read = std::chrono::steady_clock::now();

    std::ifstream file("numeros_binarios.txt", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<int> arr(fileSize / sizeof(int));
    file.read(reinterpret_cast<char*>(arr.data()), fileSize);
    file.close();
    auto end_read = std::chrono::steady_clock::now();

    auto start_sort = std::chrono::steady_clock::now();
    parallelHeapSort(arr, arr.size());
    auto end_sort = std::chrono::steady_clock::now();

    auto start_write = std::chrono::steady_clock::now();
    std::ofstream outFile("numeros_ordenados_binarios.txt", std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo para escribir." << std::endl;
        return 1;
    }
    for (int num : arr) {
        outFile.write(reinterpret_cast<const char*>(&num), sizeof(int));
    }
    outFile.close();
    auto end_write = std::chrono::steady_clock::now();

    auto end_total = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds_read = end_read - start_read;
    std::chrono::duration<double> elapsed_seconds_sort = end_sort - start_sort;
    std::chrono::duration<double> elapsed_seconds_write = end_write - start_write;
    std::chrono::duration<double> elapsed_seconds_total = end_total - start_total;

    std::cout << "Tiempo de lectura del archivo: " << elapsed_seconds_read.count() << " segundos" << std::endl;
    std::cout << "Tiempo de ordenamiento: " << elapsed_seconds_sort.count() << " segundos" << std::endl;
    std::cout << "Tiempo de escritura del archivo: " << elapsed_seconds_write.count() << " segundos" << std::endl;
    std::cout << "Tiempo total: " << elapsed_seconds_total.count() << " segundos" << std::endl;

    std::cout << "Los numeros ordenados han sido escritos en el archivo 'numeros_ordenados_binarios.txt'" << std::endl;

    return 0;
}
