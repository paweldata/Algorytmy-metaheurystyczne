#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <map>

const double MUTATION = 0.2;

int getResult(const std::string& path, std::vector<std::vector<int>>& map, int currI, int currJ);

int main(int argc, char *argv[]) {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    std::random_device rd;
    std::mt19937 generator(rd());

    double time;
    int rows;
    int columns;
    int startI = 0;
    int startJ = 0;
    int startPopulation;
    int parentsAmount;
    int pathLength = 0;
    int counter = 0;
    int sentinel = 500;

    scanf("%lf %d %d %d %d", &time, &rows, &columns, &startPopulation, &parentsAmount);

    int childrenAmount = parentsAmount * 10;
    int populationAmount = parentsAmount + childrenAmount;
    std::vector<std::vector<int> > map(rows, std::vector<int>(columns));
    std::vector<char> move{'D', 'U', 'R', 'L'};

    std::vector<std::string> population(populationAmount);
    std::vector<int> results(populationAmount);
    std::vector<int> probability(populationAmount);
    std::vector<std::string> alive(parentsAmount);

    for (int i = 0; i < rows; i++) {
        int j = 0;
        while (j < columns) {
            int c = getchar();
            if (c == 53) {
                startI = i;
                startJ = j;
            }
            if (c >= 48) {
                map[i][j++] = c - 48;
            }
        }
    }

    for (int i = 0; i < startPopulation; i++) {
        std::cin >> alive[i];
    }

    std::uniform_int_distribution<int> randCopyParent(0, startPopulation - 1);

    for (int i = startPopulation; i < parentsAmount; i++) {
        int index = randCopyParent(generator);
        alive[i] = alive[index];
    }

    std::uniform_int_distribution<int> randParent(0, parentsAmount - 1);
    std::uniform_int_distribution<int> randMove(0, 3);
    std::uniform_real_distribution<double> randFrom0To1(0, 1);

    for (int i = 0; i < parentsAmount; i++) {
        results[i] = getResult(alive[i], map, startI, startJ);
        alive[i] = alive[i].substr(0, results[i]);
    }

    int randAnswer = 0;

    // main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        int maxPathLength = 0;

        for (int i = 0; i < parentsAmount; i++) {
            population[i] = alive[i];
            maxPathLength = std::max(maxPathLength, results[i]);
        }

        // create new generation
        for (int i = parentsAmount; i < populationAmount; i++) {
            int parent = randParent(generator);
            int size = population[parent].size();
            population[i].clear();

            for (int j = 0; j < size; j++) {
                if (MUTATION >= randFrom0To1(generator)) { // mutation
                    population[i].push_back(move[randMove(generator)]);
                } else {
                    population[i].push_back(population[parent][j]);
                }
            }

            results[i] = getResult(population[i], map, startI, startJ);
            population[i] = population[i].substr(0, results[i]);
            maxPathLength = std::max(maxPathLength, results[i]);
        }

        // prepare probability values
        for (int i = 0; i < populationAmount; i++) {
            probability[i] = maxPathLength + 1 - results[i];

            if (i > 0)
                probability[i] += probability[i - 1];
        }

        // choose next generation
        std::uniform_int_distribution<int> chooseNextGeneration(0, probability[populationAmount - 1]);

        for (int i = 0; i < parentsAmount; i++) {
            int result = chooseNextGeneration(generator);
            int j = 0;

            while (probability[j] < result)
                j++;

            alive[i] = population[j];
            results[i] = getResult(alive[i], map, startI, startJ);
        }

        randAnswer = randParent(generator);

        // check if it was no change for a long time
        if (pathLength != results[randAnswer]) {
            pathLength = results[randAnswer];
            counter = 0;
        } else if (++counter >= sentinel) {
            break;
        }

        currTime = std::chrono::steady_clock::now();
    }

    printf("%d\n", results[randAnswer]);
    fprintf(stderr, "%s\n", alive[randAnswer].c_str());

}

int getResult(const std::string& path, std::vector<std::vector<int>>& map, int currI, int currJ) {
    int moves = 0;
    for (char move : path) {
        switch (move) {
            case 'U': currI--; break;
            case 'D': currI++; break;
            case 'L': currJ--; break;
            case 'R': currJ++; break;
        }

        moves++;

        if (map[currI][currJ] == 1)
            return map.size() * map[0].size();
        if (map[currI][currJ] == 8)
            return moves;
    }

    return map.size() * map[0].size();
}
