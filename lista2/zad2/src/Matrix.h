#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

#include "Sector.h"

class Matrix {
public:
    Matrix(const std::vector<std::vector<uint8_t> >&arr, int r, int c, int k);
    void changeMatrix(const std::vector<std::vector<uint8_t> >&arr, double temperature);
    double getBestDistance();
    void showBestMatrixOnStderr();

private:
    std::vector<std::vector<Sector> > array;
    std::vector<std::vector<Sector> > bestArray;
    int rows;
    int columns;
    int verSectors;
    int horSectors;
    int k;
    double distance;
    double bestDistance;

    void calculateDistance(const std::vector<std::vector<uint8_t> >&arr);
    void createArray(const std::vector<std::vector<uint8_t> >&arr);
    void changeSectorValue(const std::vector<std::vector<uint8_t> >&arr, double temperature, int x, int y);
    void changeSectorSize(const std::vector<std::vector<uint8_t> >&arr, double temperature, int x, int y);
    void setBestAnswer();
};

#endif //MATRIX_H
