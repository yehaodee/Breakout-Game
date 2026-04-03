#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include "raylib.h"
#include "GameObject.h"

class VisualObject : public virtual GameObject {
protected:
    Color color;
    bool visible;
public:
    VisualObject(Color c, bool v = true);
};
#endif