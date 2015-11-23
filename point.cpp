
#include "point.h"

point3d::point3d()
{
  init(0,0,0);
}

point3d::point3d(double px, double py, double pz)
{
  init(px, py, pz);
}

void point3d::init(double px, double py, double pz)
{
  x = px;
  y = py;
  z = pz;
}