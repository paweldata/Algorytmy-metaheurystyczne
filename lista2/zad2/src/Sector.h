#ifndef SECTOR_H
#define SECTOR_H

#include <cstdint>

class Sector {
public:
    Sector() {}
    Sector(int i, int j, int rows, int columns);

    void setRandomValue();
    void setValue(uint8_t value);
    void setJ(int j);
    void setColumns(int columns);
    void setRows(int rows);
    void setI(int i);

    uint8_t getValue() const;
    int getI() const;
    int getJ() const;
    int getRows() const;
    int getColumns() const;

private:
    static uint8_t values[];

    uint8_t value{};
    int i;
    int j;
    int rows;
    int columns;
};

#endif //SECTOR_H
