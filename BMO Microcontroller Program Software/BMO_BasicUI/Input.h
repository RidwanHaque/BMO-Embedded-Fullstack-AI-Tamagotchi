#ifndef INPUT_H
#define INPUT_H

// Latched button timestamps (Snake resets these on game start)
extern unsigned long lastUp;
extern unsigned long lastDown;
extern unsigned long lastLeft;
extern unsigned long lastRight;
extern unsigned long lastCenter;

// Latched nav read function
void readNavLatched(bool &up, bool &down, bool &left, bool &right, bool &center);

#endif
