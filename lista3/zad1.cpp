#include <cmath>
#include <chrono>
#include <cstdio>
#include <random>
#include <array>

const int SIZE = 5;
const int PARENTS = 5;
const int CHILDREN = 10;
const int POPULATION = PARENTS + CHILDREN;
const double MUTATION = 0.1;

double XSYang(std::array<double, SIZE> x, std::array<double, SIZE> ex);

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> randChange(-0.05, 0.05);
    std::uniform_int_distribution<int> randParent(0, PARENTS);
    std::uniform_int_distribution<int> randIndex(0, SIZE - 1);
    std::uniform_real_distribution<double> randFrom0To1(0, 1);

    int time;
    std::array<double, SIZE> ex{};
    std::vector<std::array<double, SIZE> > all(POPULATION, std::array<double, SIZE>{});
    std::vector<std::array<double, SIZE> > alive(PARENTS, std::array<double, SIZE>{});
    std::vector<double> results(POPULATION);
    std::vector<double> probability(POPULATION);

    std::scanf("%d", &time);
    for (int j = 0; j < SIZE; j++) {
        scanf("%lf", &alive[0][j]);

        for (int i = 1; i < PARENTS; i++)
            alive[i][j] = alive[0][j];
    }

    for (int i = 0; i < SIZE; i++)
        scanf("%lf", &ex[i]);

    int randResult = 0;

    // main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        double maxValue = 0;

        for (int i = 0; i < PARENTS; i++) {
            all[i] = alive[i];
            results[i] = XSYang(all[i], ex);
            maxValue = std::max(maxValue, results[i]);
        }

        // create new generation
        for (int i = PARENTS; i < POPULATION; i++) {
            int parent1 = randParent(generator);
            int parent2 = randParent(generator);
            int index = randIndex(generator);

            for (int j = 0; j < index; j++) {
                all[i][j] = all[parent1][j];
                if (MUTATION >= randFrom0To1(generator)) //mutation
                    all[i][j] += randChange(generator);
            }

            for (int j = index; j < SIZE; j++) {
                all[i][j] = all[parent2][j];
                if (MUTATION >= randFrom0To1(generator)) //mutation
                    all[i][j] += randChange(generator);
            }

            results[i] = XSYang(all[i], ex);
            maxValue = std::max(maxValue, results[i]);
        }

        // prepare probability values
        for (int i = 0; i < POPULATION; i++) {
            probability[i] = pow(maxValue - results[i], 2);

            if (i > 0)
                probability[i] += probability[i - 1];
        }

        // choose next generation
        std::uniform_real_distribution<double> chooseNextGeneration(0, probability[POPULATION - 1]);

        for (int i = 0; i < PARENTS; i++) {
            double result = chooseNextGeneration(generator);

            int j = 0;
            while (probability[j] <= result)
                j++;

            alive[i] = all[j];
            results[i] = XSYang(alive[i], ex);
        }

        randResult = randParent(generator);

        if (results[randResult] <= 0.00001)
            break;

        currTime = std::chrono::steady_clock::now();
    }

    for (int j = 0; j < SIZE; j++)
        printf("%f ", alive[randResult][j]);
    printf("%f\n", results[randResult]);

    return 0;
}

double XSYang(const std::array<double, SIZE> x, std::array<double, SIZE> ex) {
    double result = 0;

    for (int i = 0; i < SIZE; i++)
        result += ex[i] * pow(std::abs(x[i]), i + 1);

    return result;
}
