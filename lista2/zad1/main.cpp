#include <cmath>
#include <chrono>
#include <cstdio>
#include <random>
#include <array>

const int SIZE = 4;
const double DECREASVALUE = 0.95;
const double NEIGHBOORHOOD = 1.0;

double salomon(std::array<double, SIZE> x);

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> randNeighbor(-NEIGHBOORHOOD, NEIGHBOORHOOD);
    std::uniform_real_distribution<> rand0to1(0, 1);
    std::uniform_int_distribution<> randIndex(0, SIZE - 1);

    int time;
    std::array<double, SIZE> x{};

    std::scanf("%d", &time);
    for (int i = 0; i < SIZE; i++)
        scanf("%lf", &x[i]);

    std::array<double, SIZE> xBest = x;
    double xValue = salomon(x);
    double xBestValue = xValue;
    int maxTries = time * 5000;
    double temperature = sqrt(time);

    //main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        for (int tries = 0; tries < maxTries; tries++) {
            int index = randIndex(generator);
            double value = randNeighbor(generator);
            double saveValue = x[index];
            x[index] += value;

            double xNewValue = salomon(x);
            if (xNewValue < xValue) {
                xValue = xNewValue;
                if (xValue < xBestValue) {
                    xBest = x;
                    xBestValue = xValue;
                }
            } else if (rand0to1(generator) < 1.0 / ( 1 + exp((xNewValue - xValue) / temperature))) {
                xValue = xNewValue;
            } else {
                x[index] = saveValue;
            }
        }

        temperature *= DECREASVALUE;
        currTime = std::chrono::steady_clock::now();
    }

    for(double value : xBest)
        printf("%f ", value);
    printf("%f\n", xBestValue);

    return 0;
}

double salomon(const std::array<double, SIZE> x) {
    double sumx2sqrt = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]);
    return 1 - cos(2 * M_PI * sumx2sqrt) + sumx2sqrt / 10.0;
}
