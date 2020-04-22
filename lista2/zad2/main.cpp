#include <chrono>
#include <cstdio>

#include "src/Matrix.h"

const double DECREASVALUE = 0.999;

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();
    int time;
    int rows;
    int columns;
    int k;

    scanf("%d %d %d %d", &time, &rows, &columns, &k);

    double temperature = rows * columns * 255;
    int tries = rows * columns;

    std::vector<std::vector<uint8_t> >array(rows, std::vector<uint8_t >(columns));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            scanf("%hhd", &array[i][j]);

    Matrix matrix(array, rows, columns, k);

    //main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        for (int i = 0; i < tries; i++)
            matrix.changeMatrix(array, temperature);

        temperature *= DECREASVALUE;
        currTime = std::chrono::steady_clock::now();
    }

    printf("%f\n", matrix.getBestDistance());
    matrix.showBestMatrixOnStderr();

    return 0;
}
