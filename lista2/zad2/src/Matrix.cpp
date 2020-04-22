#include <random>

#include "Matrix.h"

Matrix::Matrix(const std::vector<std::vector<uint8_t> > &arr, int r, int c, int k) {
    this->rows = r;
    this->columns = c;
    this->k = k;
    this->bestDistance = -1;

    this->createArray(arr);
    this->calculateDistance(arr);

    this->bestDistance = this->distance;
    this->bestArray = this->array;
}

void Matrix::changeMatrix(const std::vector<std::vector<uint8_t> >&arr, double temperature) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randomVerSector(0, this->verSectors - 1);
    std::uniform_int_distribution<> randomHorSector(0, this->horSectors - 1);
    std::uniform_int_distribution<> randomInt1or2(1, 2);

    int x = randomVerSector(generator);
    int y = randomHorSector(generator);

    switch (randomInt1or2(generator)) {
        case 1: changeSectorValue(arr, temperature, x, y); break;
        case 2: changeSectorSize(arr, temperature, x, y); break;
    }
}

double Matrix::getBestDistance() {
    return this->bestDistance;
}

void Matrix::showBestMatrixOnStderr() {
    std::vector<std::vector<uint8_t> >newArray(this->rows, std::vector<uint8_t>(this->columns));

    for (int x = 0; x < this->verSectors; x++) {
        for (int y = 0; y < this->horSectors; y++) {
            Sector currSector = this->bestArray[x][y];
            int sectorI = currSector.getI();
            int sectorJ = currSector.getJ();
            int maxRow = currSector.getRows() + sectorI;
            int maxColumn = currSector.getColumns() + sectorJ;
            int value = currSector.getValue();
            for (int i = sectorI; i < maxRow; i++) {
                for (int j = sectorJ; j < maxColumn; j++) {
                     newArray[i][j] = value;
                }
            }
        }
    }

    for (int i = 0; i < this->rows; i++) {
        for (int j = 0; j < this->columns; j++)
            fprintf(stderr, "%d ", newArray[i][j]);
        fprintf(stderr, "\n");
    }
}

void Matrix::calculateDistance(const std::vector<std::vector<uint8_t> > &arr) {
    uint64_t newDistance = 0;

    for (int x = 0; x < this->verSectors; x++) {
        for (int y = 0; y < this->horSectors; y++) {
            Sector currSector = this->array[x][y];
            int sectorI = currSector.getI();
            int sectorJ = currSector.getJ();
            int maxRow = currSector.getRows() + sectorI;
            int maxColumn = currSector.getColumns() + sectorJ;
            int value = currSector.getValue();
            for (int i = sectorI; i < maxRow; i++) {
                for (int j = sectorJ; j < maxColumn; j++) {
                    newDistance += pow(arr[i][j] - value, 2);
                }
            }
        }
    }

    this->distance = double(newDistance) / (this->rows * this->columns);
}

void Matrix::createArray(const std::vector<std::vector<uint8_t> >&arr) {
    this->verSectors = this->rows / this->k;
    this->horSectors = this->columns / this->k;
    int verUpLimit = this->rows - (verSectors * this->k);
    int horUpLimit = this->columns - (horSectors * this->k);
    int horUpLimitSave = horUpLimit;

    this->array = std::vector<std::vector<Sector> >(this->verSectors, std::vector<Sector>(this->horSectors, Sector()));

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randomInt(0, std::max(horUpLimit, verUpLimit));

    for (int i = 0, x = 0; x < verSectors; x++) {
        int moreRows = (verUpLimit > 0) ? randomInt(generator) % (verUpLimit + 1) : 0;
        verUpLimit -= moreRows;

        if (x + 1 == verSectors)
            moreRows += verUpLimit;

        horUpLimit = horUpLimitSave;
        for (int j = 0, y = 0; y < horSectors; y++) {
            int moreColumns = (horUpLimit > 0) ? randomInt(generator) % (horUpLimit + 1) : 0;
            horUpLimit -= moreColumns;

            if (y + 1 == horSectors)
                moreColumns += horUpLimit;

            this->array[x][y] = Sector(i, j, k + moreRows, k + moreColumns);
            j += k + moreColumns;
        }

        i += k + moreRows;
    }
}

void Matrix::changeSectorValue(const std::vector<std::vector<uint8_t> > &arr, double temperature, int x, int y) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> randomFrom0to1(0, 1);

    int64_t currValue = 0;

    Sector currSector = this->array[x][y];
    int sectorI = currSector.getI();
    int sectorJ = currSector.getJ();
    int maxRow = currSector.getRows() + sectorI;
    int maxColumn = currSector.getColumns() + sectorJ;

    uint8_t oldValue = currSector.getValue();
    currSector.setRandomValue();
    uint8_t newValue = currSector.getValue();

    for (int i = sectorI; i < maxRow; i++)
        for (int j = sectorJ; j < maxColumn; j++)
            currValue += pow(arr[i][j] - newValue, 2) - pow(arr[i][j] - oldValue, 2);

    if (currValue < 0) {
        this->array[x][y].setValue(newValue);
        this->calculateDistance(arr);
        this->setBestAnswer();
    } else if (randomFrom0to1(generator) < 1.0 / ( 1.0 + exp(double(currValue) / temperature))) {
        this->array[x][y].setValue(newValue);
        this->calculateDistance(arr);
    }
}

void Matrix::changeSectorSize(const std::vector<std::vector<uint8_t> >&arr, double temperature, int x, int y) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randomSide(1, 4);
    std::uniform_real_distribution<> randomFrom0to1(0, 1);

    Sector currSector = this->array[x][y];
    uint8_t newValue = currSector.getValue();
    int64_t currValue = 0;

    switch (randomSide(generator)) {
        case 1: { // north
            if (x == 0)
                return;
            Sector nextSector = this->array[x - 1][y];
            uint8_t oldValue = nextSector.getValue();

            if (nextSector.getRows() == this->k)
                return;

            if (currSector.getJ() != nextSector.getJ() || currSector.getColumns() != nextSector.getColumns())
                return;

            int maxColumn = currSector.getColumns() + currSector.getJ();
            int i = currSector.getI() - 1;

            for (int j = currSector.getJ(); j < maxColumn; j++)
                currValue += pow(arr[i][j] - newValue, 2) - pow(arr[i][j] - oldValue, 2);

            if (currValue < 0) {
                this->array[x][y].setI(i);
                this->array[x][y].setRows(currSector.getRows() + 1);
                this->array[x - 1][y].setRows(nextSector.getRows() - 1);

                this->calculateDistance(arr);
                this->setBestAnswer();
            } else if (randomFrom0to1(generator) < 1.0 / (1.0 + exp(double(currValue) / temperature))) {
                this->array[x][y].setI(i);
                this->array[x][y].setRows(currSector.getRows() + 1);
                this->array[x - 1][y].setRows(nextSector.getRows() - 1);

                this->calculateDistance(arr);
            }

            break;
        } case 2: { // south
            if (x == this->verSectors - 1)
                return;
            Sector nextSector = this->array[x + 1][y];
            uint8_t oldValue = nextSector.getValue();

            if (nextSector.getRows() == this->k)
                return;

            if (currSector.getJ() != nextSector.getJ() || currSector.getColumns() != nextSector.getColumns())
                return;

            int maxColumn = currSector.getColumns() + currSector.getJ();
            int i = nextSector.getI();

            for (int j = currSector.getJ(); j < maxColumn; j++)
                currValue += pow(arr[i][j] - newValue, 2) - pow(arr[i][j] - oldValue, 2);

            if (currValue < 0) {
                this->array[x][y].setRows(currSector.getRows() + 1);
                this->array[x + 1][y].setI(i + 1);
                this->array[x + 1][y].setRows(nextSector.getRows() - 1);

                this->calculateDistance(arr);
                this->setBestAnswer();
            } else if (randomFrom0to1(generator) < 1.0 / (1.0 + exp(double(currValue) / temperature))) {
                this->array[x][y].setRows(currSector.getRows() + 1);
                this->array[x + 1][y].setI(i + 1);
                this->array[x + 1][y].setRows(nextSector.getRows() - 1);

                this->calculateDistance(arr);
            }
            break;
        } case 3: { // west
            if (y == 0)
                return;
            Sector nextSector = this->array[x][y - 1];
            uint8_t oldValue = nextSector.getValue();

            if (nextSector.getColumns() == this->k)
                return;

            if (currSector.getI() != nextSector.getI() || currSector.getRows() != nextSector.getRows())
                return;

            int maxRows = currSector.getRows() + currSector.getI();
            int j = currSector.getJ() - 1;

            for (int i = currSector.getI(); i < maxRows; i++)
                currValue += pow(arr[i][j] - newValue, 2) - pow(arr[i][j] - oldValue, 2);

            if (currValue < 0) {
                this->array[x][y].setJ(j);
                this->array[x][y].setColumns(currSector.getColumns() + 1);
                this->array[x][y - 1].setColumns(nextSector.getColumns() - 1);

                this->calculateDistance(arr);
                this->setBestAnswer();
            } else if (randomFrom0to1(generator) < 1.0 / (1.0 + exp(double(currValue) / temperature))) {
                this->array[x][y].setJ(j);
                this->array[x][y].setColumns(currSector.getColumns() + 1);
                this->array[x][y - 1].setColumns(nextSector.getColumns() - 1);

                this->calculateDistance(arr);
            }
            break;
        } case 4: { // east
            if (y == this->horSectors- 1)
                return;
            Sector nextSector = this->array[x][y + 1];
            uint8_t oldValue = nextSector.getValue();

            if (nextSector.getColumns() == this->k)
                return;

            if (currSector.getI() != nextSector.getI() || currSector.getRows() != nextSector.getRows())
                return;

            int maxRows = currSector.getRows() + currSector.getI();
            int j = nextSector.getJ();

            for (int i = currSector.getI(); i < maxRows; i++)
                currValue += pow(arr[i][j] - newValue, 2) - pow(arr[i][j] - oldValue, 2);

            if (currValue < 0) {
                this->array[x][y].setColumns(currSector.getColumns() + 1);
                this->array[x][y + 1].setJ(j + 1);
                this->array[x][y + 1].setColumns(nextSector.getColumns() - 1);

                this->calculateDistance(arr);
                this->setBestAnswer();
            } else if (randomFrom0to1(generator) < 1.0 / (1.0 + exp(double(currValue) / temperature))) {
                this->array[x][y].setColumns(currSector.getColumns() + 1);
                this->array[x][y + 1].setJ(j + 1);
                this->array[x][y + 1].setColumns(nextSector.getColumns() - 1);

                this->calculateDistance(arr);
            }
            break;
        }
    }
}

void Matrix::setBestAnswer() {
    if (this->distance < this->bestDistance || this->bestDistance == -1) {
        this->bestDistance = this->distance;
        this->bestArray = this->array;
    }
}
