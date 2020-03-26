#include <cstdio>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

const int TABURATIO = 1;
const int BIGJUMP = 5;

std::vector<int> getRandomPermutation(std::vector<int>&tab, int size) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randInt(1, size);

    std::vector<int> temp(size);
    std::vector<int> permutation(size);
    int value;
    int currSize = size;

    for (int i = 0; i < size; i++)
        temp[i] = i + 1;

    int i = 0;
    while (i < size) {
        value = randInt(generator) % currSize;
        permutation[i] = temp[value];

        //check, if this egde was in previous permutation
        if (i == 0) {
            if (tab[0] != permutation[0]) {
                std::swap(temp[value], temp[currSize - 1]);
                currSize--;
                i++;
            }
        } else if (currSize > 1 && tab[i] > 0) {
            for (int j = 0; j < size; j++) {
                if (tab[j] == permutation[i]) {
                    if (j == 0 || tab[j - 1] == 0 || tab[j - 1] != permutation[i - 1]) {
                        std::swap(temp[value], temp[currSize - 1]);
                        currSize--;
                        i++;
                    }
                    j = size;
                }
            }
        } else {
            std::swap(temp[value], temp[currSize - 1]);
            currSize--;
            i++;
        }
    }

    return permutation;
}

int countValue(std::vector<std::vector<int> >& tab, std::vector<int>& tempAnswer, int size) {
    int answer = tab[0][tempAnswer[0]];

    for (int i = 0; i < size - 1; i++)
        answer += tab[tempAnswer[i]][tempAnswer[i + 1]];

    return answer + tab[tempAnswer[size - 1]][0];
}

bool checkTabu(std::vector<std::vector<int> >&tabu, std::vector<int>&current, int i, int j, int sentinel, int tries) {
    int previousVertex = (i == 0) ? 0: current[i - 1];
    int nextVertex = (j == current.size() - 1) ? 0: current[j + 1];

    return (tries - tabu[previousVertex][current[j]] > sentinel &&
            tries - tabu[current[j]][current[i + 1]] > sentinel &&
            tries - tabu[current[j + 1]][current[i]] > sentinel &&
            tries - tabu[current[i]][nextVertex] > sentinel);
}

void updateTabu(std::vector<std::vector<int> >&tabu, std::vector<int>&current, int i, int j, int tries) {
    int previousVertex = (i == 0) ? 0: current[i - 1];
    int nextVertex = (j == current.size() - 1) ? 0: current[j + 1];

    tabu[previousVertex][current[i]] = tries;
    tabu[current[i]][current[i + 1]] = tries;
    tabu[current[j - 1]][current[j]] = tries;
    tabu[current[j]][nextVertex] = tries;
}

void updateSwapCounter(std::vector<std::vector<int> >&swapCounter, std::vector<int>&current, int i, int j) {
    int previousVertex = (i == 0) ? 0: current[i - 1];
    int nextVertex = (j == current.size() - 1) ? 0: current[j + 1];

    swapCounter[previousVertex][current[i]]++;
    swapCounter[current[i]][current[i + 1]]++;
    swapCounter[current[j - 1]][current[j]]++;
    swapCounter[current[j]][nextVertex]++;
}

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();
    int time;
    int number;
    int bestValue = -1;
    int currentValue;
    int bestCurrentValue = 0;
    int bestSwapi = 0;
    int bestSwapj = 0;
    int bigJumpCount;

    scanf("%d %d", &time, &number);

    int tabuSentinel = 3 * number;
    int tries = int(sqrt(number) * 100);
    int size = number - 1;

    std::vector<std::vector<int> > tab(number, std::vector<int>(number, 0));
    std::vector<std::vector<int> > swapCounter(number, std::vector<int>(number));
    std::vector<std::vector<int> > tabu(number, std::vector<int>(number));
    std::vector<int> best(size);
    std::vector<int> current(size, 0);

    for (int i = 0; i < number; i++) {
        for (int j = 0; j < number; j++)
            scanf("%d", &tab[i][j]);
    }

    //main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        current = getRandomPermutation(current, size);
        bigJumpCount = 0;

        //reset swapCounter values to 0, tabu to 0 - tabuSentinel
        for (int i = 0; i < size; i++) {
            std::fill(swapCounter[i].begin(), swapCounter[i].end(), 0);
            std::fill(tabu[i].begin(), tabu[i].end(), 0 - tabuSentinel);
        }

        for (int currTry = 1; currTry <= tries; currTry++) {

            //try all swaps using tabu
            bestCurrentValue = countValue(tab, current, size);
            currentValue = bestCurrentValue;
            for (int i = 0; i < size - 1; i++) {
                for (int j = i + 1; j < size; j++) {
                    //check tabu value
                    if (checkTabu(tabu, current, i, j, tabuSentinel, tries)) {
                        int newValue = currentValue;
                        int previousVertex = (i == 0) ? 0: current[i - 1];
                        int nextVertex = (j == size - 1) ? 0: current[j + 1];

                        if (i + 1 == j) {
                            newValue -= tab[previousVertex][current[i]] + tab[current[i]][current[j]]
                                    + tab[current[j]][nextVertex];
                            newValue += tab[previousVertex][current[j]] + tab[current[j]][current[i]]
                                    + tab[current[i]][nextVertex];
                        } else {
                            newValue -= tab[previousVertex][current[i]] + tab[current[i]][current[i + 1]]
                                        + tab[current[j - 1]][current[j]] + tab[current[j]][nextVertex];
                            newValue += tab[previousVertex][current[j]] + tab[current[j]][current[i + 1]]
                                        + tab[current[j - 1]][current[i]] + tab[current[i]][nextVertex];
                        }

                        //check if it is better then other value
                        if (newValue < bestCurrentValue) {
                            bestSwapi = i;
                            bestSwapj = j;
                            bestCurrentValue = newValue;
                        }
                    }
                }
            }

            //check if better solution was found
            if (bestCurrentValue < currentValue) {
                //choose better solution
                updateTabu(tabu, current, bestSwapi, bestSwapj, tries);
                std::swap(current[bestSwapi], current[bestSwapj]);
                updateSwapCounter(swapCounter, current, bestSwapi, bestSwapj);
            } else {
                //choose the best solution from tabu

                for (int i = 0; i < size - 1; i++) {
                    for (int j = i + 1; j < size; j++) {

                        //check tabu value
                        if (!checkTabu(tabu, current, i, j, tabuSentinel, tries)) {
                            int newValue = currentValue;
                            int previousVertex = (i == 0) ? 0: current[i - 1];
                            int nextVertex = (j == size - 1) ? 0: current[j + 1];

                            if (i + 1 == j) {
                                newValue -= tab[previousVertex][current[i]] + tab[current[i]][current[j]]
                                            + tab[current[j]][nextVertex];
                                newValue += tab[previousVertex][current[j]] + tab[current[j]][current[i]]
                                            + tab[current[i]][nextVertex];
                                newValue += TABURATIO * (swapCounter[previousVertex][current[j]]
                                        + swapCounter[current[j]][current[i]] + swapCounter[current[i]][nextVertex]);
                            } else {
                                newValue -= tab[previousVertex][current[i]] + tab[current[i]][current[i + 1]]
                                            + tab[current[j - 1]][current[j]] + tab[current[j]][nextVertex];
                                newValue += tab[previousVertex][current[j]] + tab[current[j]][current[i + 1]]
                                            + tab[current[j - 1]][current[i]] + tab[current[i]][nextVertex];
                                newValue += TABURATIO * (swapCounter[previousVertex][current[j]]
                                        + swapCounter[current[j]][current[i + 1]]+
                                        swapCounter[current[j - 1]][current[i]] + swapCounter[current[i]][nextVertex]);
                            }

                            //check if it is better then other value
                            if (newValue < bestCurrentValue) {
                                bestSwapi = i;
                                bestSwapj = j;
                                bestCurrentValue = newValue;
                            }
                        }
                    }
                }

                if (bestCurrentValue < currentValue) {
                    //choose better solution
                    updateTabu(tabu, current, bestSwapi, bestSwapj, tries);
                    std::swap(current[bestSwapi], current[bestSwapj]);
                    updateSwapCounter(swapCounter, current, bestSwapi, bestSwapj);

                } else if (bigJumpCount++ < BIGJUMP) {
                    //do random jump
                    std::random_device rd;
                    std::mt19937 generator(rd());
                    std::uniform_int_distribution<> randInt(0, size - 1);

                    for (int i = 0; i < BIGJUMP; i++) {
                        int index1 = randInt(generator);
                        int index2 = randInt(generator);
                        updateTabu(tabu, current, index1, index2, tries);
                        std::swap(current[index1], current[index2]);
                        updateSwapCounter(swapCounter, current, index1, index2);
                    }
                } else {
                    currTry = tries + 1;
                }
            }
        }

        if (bestValue == -1 || bestCurrentValue < bestValue) {
            best = current;
            bestValue = bestCurrentValue;
        }

        currTime = std::chrono::steady_clock::now();
    }

    printf("%d", bestValue);

    fprintf(stderr,"1 ");
    for (int i = 0; i < size; i++)
        fprintf(stderr,"%d ", best[i] + 1);
    fprintf(stderr,"1");

    return 0;
}
