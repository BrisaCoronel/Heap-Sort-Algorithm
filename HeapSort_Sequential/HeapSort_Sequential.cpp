#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>

using namespace std;

void heapify(vector<int>& arr, int n, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != index) {
        swap(arr[index], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr) {
    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

int main() {
    auto start_total = chrono::steady_clock::now();
    auto start_read = chrono::steady_clock::now();
    ifstream file("numeros_binarios.txt", ios::binary);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return 1;
    }

    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<int> arr(fileSize / sizeof(int));
    file.read(reinterpret_cast<char*>(arr.data()), fileSize);
    file.close();
    auto end_read = chrono::steady_clock::now();

    auto start_sort = chrono::steady_clock::now();
    heapSort(arr);
    auto end_sort = chrono::steady_clock::now();

    auto start_write = chrono::steady_clock::now();
    ofstream outFile("numeros_ordenados_binarios.txt");
    if (!outFile.is_open()) {
        cerr << "Error al abrir el archivo para escribir." << endl;
        return 1;
    }
    for (int num : arr) {
        outFile.write(reinterpret_cast<const char*>(&num), sizeof(int));
    }
    outFile.close();
    auto end_write = chrono::steady_clock::now();

    auto end_total = chrono::steady_clock::now();

    chrono::duration<double> elapsed_seconds_read = end_read - start_read;
    chrono::duration<double> elapsed_seconds_sort = end_sort - start_sort;
    chrono::duration<double> elapsed_seconds_write = end_write - start_write;
    chrono::duration<double> elapsed_seconds_total = end_total - start_total;

    cout << "Tiempo de lectura del archivo: " << elapsed_seconds_read.count() << " segundos" << endl;
    cout << "Tiempo de ordenamiento: " << elapsed_seconds_sort.count() << " segundos" << endl;
    cout << "Tiempo de escritura del archivo: " << elapsed_seconds_write.count() << " segundos" << endl;
    cout << "Tiempo total: " << elapsed_seconds_total.count() << " segundos" << endl;

    cout << "Los numeros ordenados han sido escritos en el archivo 'numeros_ordenados_binarios.txt'" << endl;

    return 0;
}