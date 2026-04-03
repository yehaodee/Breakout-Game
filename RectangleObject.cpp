#include "RectangleObject.h"

RectangleObject::RectangleObject(float x, float y, float w, float h) : GameObject({x, y}) {
    rect = {x, y, w, h};
}
