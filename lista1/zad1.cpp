#include <cstdio>
#include <cmath>
#include <chrono>
#include <random>

const int SIZE = 4;
const int TRIES = 50;
const int BIGJUMP = 2;
const double PRECISION = 0.0002;
const double NEIGHBORHOOD_RANGE = 0.001;

double happyCat(const double x[4]) {
    double powerNormTo2 = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3];
    return pow((powerNormTo2 - 4.0) * (powerNormTo2 - 4.0), 0.125)
        + (powerNormTo2 / 2.0 + x[0] + x[1] + x[2] + x[3]) / 4.0 + 0.5;
}

double griewank(const double x[4]) {
    return 1 + (x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]) / 4000
        - cos(x[0]) * cos(x[1] / sqrt(2)) * cos(x[2] / sqrt(3)) * cos(x[3]);
}

double* getRandomValues() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> randDouble(-2.0, 2.0);

    auto* x =(double *)(malloc(SIZE * sizeof(double)));
    for (int i = 0; i < SIZE; i++)
        x[i] = randDouble(generator);

    return x;
}

double* findMin(double* x, double(*function)(const double[])) {
    int range = int(NEIGHBORHOOD_RANGE * 2.0 / PRECISION);
    auto* xBest = (double*)malloc(SIZE * sizeof(double));
    for (int i = 0; i < SIZE; i++)
        xBest[i] = x[i];

    double currValue = function(x);
    double newValue;
    double bestValue = currValue;

    double saveValue0 = x[0];
    for (int index0 = 0; index0 < range; index0++) {
        x[0] -= range * PRECISION / 2;
        double saveValue1 = x[1];

        for (int index1 = 0; index1 < range; index1++) {
            x[1] -= range * PRECISION / 2;

            double saveValue2 = x[2];
            for (int index2 = 0; index2 < range; index2++) {
                x[2] -= range * PRECISION / 2;

                double saveValue3 = x[3];
                for (int index3 = 0; index3 < range; index3++) {
                    newValue = function(x);

                    if (newValue < bestValue) {
                        bestValue = newValue;

                        for (int i = 0; i < SIZE; i++)
                            xBest[i] = x[i];
                    }
                    x[3] += PRECISION;
                }
                x[3] = saveValue3;
                x[2] += PRECISION;
            }
            x[2] = saveValue2;
            x[1] += PRECISION;
        }
        x[1] = saveValue1;
        x[0] += PRECISION;
    }
    x[0] = saveValue0;

    return xBest;
}

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    auto * x = (double*)malloc(SIZE * sizeof(double));
    double* xNew;

    int time;
    int functionNumber;
    double (*function)(const double[]) = nullptr;
    double currValue = 0;
    double newValue;
    int counter = 0;
    int bigJUmpCounter = 0;

    scanf("%d %d", &time, &functionNumber);

    function = (functionNumber == 0) ? &happyCat : &griewank;

    //main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        x = getRandomValues();
        currValue = function(x);
        bigJUmpCounter = 0;

        for (int currTry = 1; currTry <= TRIES; currTry++) {
            //try all neighborhoods
            xNew = findMin(x, function);
            newValue = function(xNew);
            if (newValue < currValue) {
                currValue = newValue;
                x = xNew;
            } else if (bigJUmpCounter++ < BIGJUMP){
                //do random jump
                std::random_device rd;
                std::mt19937 generator(rd());
                std::uniform_int_distribution<> randInt(0, SIZE - 1);
                std::uniform_real_distribution<> randDouble(-2.0, 2.0);

                x[randInt(generator)] = randDouble(generator);
            } else {
                currTry = TRIES + 1;
            }
        }

        currTime = std::chrono::steady_clock::now();
    }

    for (int i = 0; i < SIZE; i++)
        printf("%lf ", x[i]);
    printf("%lf", currValue);

    return 0;
}
