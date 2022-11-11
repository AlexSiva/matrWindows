#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
#define NOMINMAX
#include <windows.h>

int** makeMatrix(int n) {
  int** matrix = new int*[n];
  for (int i = 0; i < n; ++i) {
    matrix[i] = new int[n];
    for (int j = 0; j < n; ++j) {
      matrix[i][j] = rand() % 10;
    }
  }
  return matrix;
}

void printMatrix(int** a, int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      std::cout << a[i][j] << ' ';
    }
    std::cout << '\n';
  }
  std::cout << '\n';
}

void blocksMult(int** a, int** b, int** c, int n, int block, int I, int J) {
  for (int i = I; i < std::min(n, I + block); ++i) {
    for (int j = J; j < std::min(n, J + block); ++j) {
      c[i][j] = 0;
      for (int k = 0; k < n; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

struct Params {
  int** A;
  int** B;
  int** C;
  int n;
  int block;
  int I;
  int J;
  Params(int** A, int** B, int** C, int n, int block, int I, int J)
      : A(A), B(B), C(C), n(n), block(block), I(I), J(J) {}
};

int threadMult(int** a, int** b, int** c, int n, int block) {
  std::vector<HANDLE> threads;
  for (int I = 0; I < n; I += block) {
    for (int J = 0; J < n; J += block) {
      Params params(a, b, c, n, block, I, J);
      threads.emplace_back(CreateThread(
          nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&blocksMult),
          &params, 0, nullptr));
    }
  }
  return threads.size();
}

void deleteMatrix(int** matrix) {
  delete[] matrix[0];
  delete[] matrix;
}

void Mult(int** a, int** b, int** c, int n, int block) {
  for (int I = 0; I < n; I += block) {
    for (int J = 0; J < n; J += block) {
      blocksMult(a, b, c, n, block, I, J);
    }
  }
}

int main() {
  int n = 6;
  int** matr = makeMatrix(n);
  int** matr2 = makeMatrix(n);
  int** matr3 = makeMatrix(n);
  printMatrix(matr, n);
  printMatrix(matr2, n);
  for (int i = 1; i <= n; i++) {
    auto start = std::chrono::steady_clock::now();
    int blocks = threadMult(matr, matr2, matr3, n, i);
    auto end = std::chrono::steady_clock::now();
    int time1 =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    auto start2 = std::chrono::steady_clock::now();
    Mult(matr, matr2, matr3, n, i);
    auto end2 = std::chrono::steady_clock::now();
    int time2 =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    std::cout << "blockSize = " << i << ", numberOfThreads = " << blocks
              << ", duration = " << time1
              << ", durationOfMultWithoutThreads = " << time2
              << ", result of mult: " << '\n';
    printMatrix(matr3, n);
  }
  deleteMatrix(matr);
  deleteMatrix(matr2);
  deleteMatrix(matr3);
}