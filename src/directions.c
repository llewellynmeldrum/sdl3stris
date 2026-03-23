#include "types.h"

#define X(dir) #dir,
const char* Direction_str[] = {
    Direction__LIST  //
};
#undef X

#define X(dir)                                                                                     \
    case dir:                                                                                      \
        return #dir;                                                                               \
        break;
const char* Direction_tostr(Direction dir) {
    switch (dir) {
        Direction__LIST  // <-- X MACRO
                default : return "UNKNOWN_DIRECTION";
        break;
    }
}
#undef X
// clang-format on -> really doesnt like xmacros
