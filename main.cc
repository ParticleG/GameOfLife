#include <types/GameOfLife.h>

using namespace std;
using namespace types;

int main() {
    GameOfLife gameOfLife(28, 28);

    gameOfLife.run();

    return 0;
}
