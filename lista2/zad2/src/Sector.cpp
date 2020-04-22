#include <random>

#include "Sector.h"

uint8_t Sector::values[] = {0, 32, 64, 128, 160, 192, 223, 255};

Sector::Sector(int i, int j, int rows, int columns) {
    this->i = i;
    this->j = j;
    this->rows = rows;
    this->columns = columns;
    this->setRandomValue();
}

uint8_t Sector::getValue() const {
    return value;
}

int Sector::getI() const {
    return this->i;
}

int Sector::getJ() const {
    return this->j;
}

int Sector::getRows() const {
    return this->rows;
}

int Sector::getColumns() const {
    return this->columns;
}

void Sector::setRandomValue() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randomValue(0, 7);
    uint8_t newValue;

    do {
        newValue = Sector::values[randomValue(generator)];
    } while (newValue == this->value);

    this->value = newValue;
}

void Sector::setValue(uint8_t value) {
    this->value = value;
}

void Sector::setI(int i) {
    this->i = i;
}

void Sector::setJ(int j) {
    this->j = j;
}

void Sector::setRows(int rows) {
    this->rows = rows;
}

void Sector::setColumns(int columns) {
    this->columns = columns;
}
