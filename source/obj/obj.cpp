#include "obj.h"

obj::obj(const color<>& color, vec2<> pos, vec2<> vel, real mass)
    : clr(color), pos(pos), vel(vel), mass(mass) {}
