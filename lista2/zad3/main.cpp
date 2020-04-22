#include <chrono>
#include <vector>
#include <random>
#include <map>

bool checkCorrectMove(const std::vector<std::vector<int> >& map, int i, int j, int move);
bool checkIfOppositeMoveIsWall(const std::vector<std::vector<int> >& map, int i, int j, int move);

int main(int argc, char *argv[]) {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> randomInt(0, 3);
    std::uniform_real_distribution<> randomFrom0to1(0, 1);

    double time;
    int rows;
    int columns;
    int startI = 0;
    int startJ = 0;

    scanf("%lf %d %d", &time, &rows, &columns);

    int maxPathSize = rows * columns;
    std::vector<std::vector<int> > map(rows, std::vector<int>(columns));
    std::map<int, char> turn = {{0, 'D'} , {1, 'U'}, {2, 'R'}, {3, 'L'}};
    std::vector<char> currentPath(maxPathSize);
    std::vector<char> bestPath;
    int bestPathLength = maxPathSize;

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

    //main loop
    while (std::chrono::duration<double>(currTime - start).count() < time) {
        int currentI = startI;
        int currentJ = startJ;
        int pathLength = 0;

        for (int i = 0; i < maxPathSize; i++) {
            int move = randomInt(generator);

            if (checkCorrectMove(map, currentI, currentJ, move)
            && (!checkIfOppositeMoveIsWall(map, currentI, currentJ, move)
            || randomFrom0to1(generator) < 1.0 / exp(double(pathLength) / bestPathLength))) {

                currentPath[pathLength++] = turn[move];

                switch (move) {
                    case 0: currentI++; break;
                    case 1: currentI--; break;
                    case 2: currentJ++; break;
                    case 3: currentJ--; break;
                    default: break;
                }

                if (pathLength == bestPathLength)
                    i = maxPathSize;

                if (map[currentI][currentJ] == 8) {
                    if (bestPathLength == 0 || pathLength < bestPathLength) {
                        bestPathLength = pathLength;
                        bestPath = currentPath;
                    }

                    i = maxPathSize;
                }
            }
        }

        currTime = std::chrono::steady_clock::now();
    }

    if (bestPathLength  < maxPathSize) {
        printf("%d\n", bestPathLength);

        for (int i = 0; i < bestPathLength; i++)
            fprintf(stderr, "%c", bestPath[i]);
    } else {
        fprintf(stderr, "No answer");
    }
}

bool checkCorrectMove(const std::vector<std::vector<int> >& map, int i, int j, int move) {
    switch(move) {
        case 0: return map[i + 1][j] != 1;
        case 1: return map[i - 1][j] != 1;
        case 2: return map[i][j + 1] != 1;
        case 3: return map[i][j - 1] != 1;
        default: return false;
    }
}

bool checkIfOppositeMoveIsWall(const std::vector<std::vector<int> >& map, int i, int j, int move) {
    switch(move) {
        case 0: return map[i - 1][j] == 1;
        case 1: return map[i + 1][j] == 1;
        case 2: return map[i][j - 1] == 1;
        case 3: return map[i][j + 1] == 1;
        default: return false;
    }
}
