#include <types/GameOfLife.h>

using namespace std;
using namespace types;

int main() {
    GameOfLife gameOfLife(25, 25);

    gameOfLife.run();

    return 0;
}
