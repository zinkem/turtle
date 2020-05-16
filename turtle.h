#ifndef TURTLE_H
#define TURTLE_H

#include <stdlib.h>

void turtleInit(void);
void turtleDumpImage(FILE *f);
void turtleHome(void);
void turtlePenUp(int flag);
void turtleForward(float dist);
void turtleRotate(float ccwDegrees);
void turtlePushState(void);
void turtlePopState(void);

#endif /* TURTLE_H */
