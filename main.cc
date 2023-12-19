#include <types/GameOfLife.h>
#include <utils/console.h>

using namespace std;
using namespace types;

int main() {
    GameOfLife gameOfLife(25, 25);
    utils::console::setConsoleSize(30, 120);

    gameOfLife.run();

    return 0;
}
