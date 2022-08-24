
#include "blocky.h"

using namespace std;
using namespace Geek;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    Blocky blocky;

    bool res;
    res = blocky.init();
    if (!res)
    {
        return 0;
    }

    blocky.mainLoop();

    return 0;
}
