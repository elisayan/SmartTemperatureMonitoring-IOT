#ifndef __WINDOW__
#define __WINDOW__

class Window {

public:
  virtual void on() = 0;
  virtual void setPosition(int angle) = 0;
  virtual void off() = 0;
};

#endif
