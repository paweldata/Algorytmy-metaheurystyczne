#include <iostream>
#include <chrono>
#include <fstream>
#include <random>
#include <vector>
#include <map>
#include <algorithm>

const double REMOVECHAR = 0.05;
const double ADDCHAR = 0.2;
const double MUTATION = 0.2;

int getValue(std::string& word, std::map<char, int>& characters, std::map<char, int>& values, std::vector<std::string> dict);

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<std::string> dict;
    std::fstream dictFile("dict.txt");

    if (!dictFile.is_open()) {
        printf("Add dict.txt to current folder\n");
        return 0;
    }

    std::string word;
    while (dictFile >> word)
        dict.push_back(word);

    std::sort(dict.begin(), dict.end());

    int time;
    int charactersAmount;
    int wordsAmount;

    scanf("%d %d %d", &time, &charactersAmount, &wordsAmount);

    int parentsAmount = std::max(10,wordsAmount);
    int childrenAmount = 2 * parentsAmount;
    int populationAmount = parentsAmount + childrenAmount;

    int previousResult = 0;
    int counter = 0;

    std::map<char, int> values;
    std::vector<std::string> population(populationAmount);
    std::vector<int> results(populationAmount);
    std::vector<int> probability(populationAmount);
    std::vector<std::string> alive(parentsAmount);
    std::map<char, int> characters;

    char character;
    int value;

    for (int i = 0; i < charactersAmount; i++) {
        std::cin >> character;
        scanf("%d",&value);
        characters[character]++;
        values[character] = value;
    }

    for (int i = 0; i < wordsAmount; i++) {
        std::cin >> alive[i];
        results[i] = getValue(alive[i], characters, values, dict);
    }

    std::uniform_int_distribution<int> randCopyParent(0, wordsAmount - 1);
    std::uniform_int_distribution<int> randParent(0, parentsAmount);
    std::uniform_real_distribution<double> randFrom0To1(0, 1);
    std::uniform_int_distribution<int> randChar(0, characters.size() - 1);

    for (int i = wordsAmount; i < parentsAmount; i++) {
        int index = randCopyParent(generator);
        alive[i] = alive[index];
        results[i] = results[index];
    }

    // main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        int maxValue = 0;

        for (int i = 0; i < parentsAmount; i++) {
            population[i] = alive[i];
            maxValue = std::max(maxValue, results[i]);

            probability[i] = pow(results[i], 2);
            if (i > 0)
                probability[i] += probability[i - 1];
        }

        std::uniform_real_distribution<double> chooseParentToClone(0, probability[parentsAmount - 1]);

        // create new generation
        for (int i = parentsAmount; i < populationAmount; i++) {
            int result = chooseParentToClone(generator);
            int parent = 0;
            while (probability[parent] <= result)
                parent++;
            int size = population[parent].size();

            population[i].clear();

            for (int j = 0; j < size; j++) {
                population[i].push_back(population[parent][j]);

                if (ADDCHAR >= randFrom0To1(generator)) {
                    auto it = characters.begin();
                    std::advance(it, randChar(generator));
                    population[i].push_back(it->first);
                }

                if (REMOVECHAR >= randFrom0To1(generator)) {
                    population[i].pop_back();
                }

                if (!population[i].empty() && MUTATION >= randFrom0To1(generator)) {
                    auto it = characters.begin();
                    std::advance(it, randChar(generator));
                    population[i][population[i].size() - 1] = it->first;
                }
            }

            results[i] = getValue(population[i], characters, values, dict);
        }

        // prepare probability values
        for (int i = 0; i < populationAmount; i++) {
            probability[i] = pow(results[i], 2);

            if (i > 0)
                probability[i] += probability[i - 1];
        }

        // choose next generation
        std::uniform_real_distribution<double> chooseNextGeneration(0, probability[populationAmount - 1]);

        for (int i = 0; i < parentsAmount; i++) {
            int result = chooseNextGeneration(generator);

            int j = 0;
            while (probability[j] <= result)
                j++;

            alive[i] = population[j];
            results[i] = getValue(alive[i], characters, values, dict);
        }

        int randResultIndex = randParent(generator);
        if (results[randResultIndex] == previousResult) {
            counter++;
        } else {
            previousResult = results[randResultIndex];
            counter = 0;
        }

        if (counter >= 30)
            break;

        currTime = std::chrono::steady_clock::now();
    }

    int maxIndex = 0;

    for (int i = 0; i < parentsAmount; i++) {
        if (results[i] > maxIndex)
            maxIndex = i;
    }

    printf("%d\n", results[maxIndex]);
    fprintf(stderr, "%s\n", alive[maxIndex].c_str());

    return 0;
}

int getValue(std::string& word, std::map<char, int>& characters, std::map<char, int>& values, std::vector<std::string> dict) {
    std::map<char, int> currCharacters;
    int value = 0;

    for (char character : word) {
        if (++currCharacters[character] > characters[character])
            return 1;
        value += values[character];
    }

    int left = 0;
    int right = (int)dict.size() - 1;
    int middle = 0;

    while (left < right) {
        middle = (left + right) / 2;
        if (dict[middle] == word) {
            break;
        } else if (dict[middle] < word) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }

    if (dict[middle] != word)
        return 3;

    return value;
}
