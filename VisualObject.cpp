#include "VisualObject.h"

VisualObject::VisualObject(Color c, bool v) : GameObject({0, 0}) {
    color = c;
    visible = v;
}
