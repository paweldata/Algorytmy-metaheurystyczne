#include <chrono>
#include <vector>
#include <random>
#include <map>

void resetTabu(std::map<int, int>&tabu) {
    for (int i = 0; i < 4; i++)
        tabu[i] = 0;
}

void setPossibleMoves(std::vector<int>& possibleMoves, int& possibleMovesSize, int i, int j,
                      std::vector<std::vector<int> >& map, std::map<int, int> tabu) {
    possibleMovesSize = 0;
    if (map[i + 1][j] != 1 && tabu[0] == 0)
        possibleMoves[possibleMovesSize++] = 0;
    if (map[i - 1][j] != 1 && tabu[1] == 0)
        possibleMoves[possibleMovesSize++] = 1;
    if (map[i][j + 1] != 1 && tabu[2] == 0)
        possibleMoves[possibleMovesSize++] = 2;
    if (map[i][j - 1] != 1 && tabu[3] == 0)
        possibleMoves[possibleMovesSize++] = 3;
}

int main() {
    auto start = std::chrono::steady_clock::now();
    auto currTime = std::chrono::steady_clock::now();

    double time;
    int rows;
    int columns;
    int startI = 0;
    int startJ = 0;

    scanf("%lf %d %d", &time, &rows, &columns);

    int pathSize = (rows + columns) * 5;
    int possibleMovesSize = 0;
    std::vector<std::vector<int> > map(rows, std::vector<int>(columns));
    std::map<int, char> turn = {{0, 'D'} , {1, 'U'}, {2, 'R'}, {3, 'L'}};
    std::map<int, int> tabu;
    std::vector<char> currentPath(pathSize);
    std::vector<char> bestPath(pathSize);
    std::vector<int> possibleMoves(4);

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

    while (std::chrono::duration<double>(currTime - start).count() < time) {
        std::random_device rd;
        std::mt19937 generator(rd());

        int currentI = startI;
        int currentJ = startJ;
        resetTabu(tabu);

        for (int i = 0; i < pathSize; i++) {
            setPossibleMoves(possibleMoves, possibleMovesSize, currentI, currentJ, map, tabu);
            if (possibleMovesSize == 0)
                break;

            std::uniform_int_distribution<> randInt(0, possibleMovesSize - 1);
            int move = possibleMoves[randInt(generator)];

            switch(move) {
                case 0: currentI++; break;
                case 1: currentI--; break;
                case 2: currentJ++; break;
                case 3: currentJ--; break;
                default: break;
            }

            currentPath[i] = turn[move];
            //set -1 to reverse move
            //So if I go left now, I'll never go right in this path
            move = (move % 2 == 0) ? move + 1: move - 1;
            tabu[move] = -1;

            if (map[currentI][currentJ] == 8) {
                //print answer
                printf("%d", i + 1);

                for (int j = 0; j <= i; j++)
                    fprintf(stderr, "%c",currentPath[j]);

                return 0;
            }
        }

        currTime = std::chrono::steady_clock::now();
    }

    printf("No answer :(");
    return 0;
}
