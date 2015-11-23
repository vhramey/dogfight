
#ifndef GUN_H
#define GUN_H


#include "MyVector.h"

#include <GLUT/glut.h>


class Gun
{
public:
	Gun();
	void fire(double x, double y, double z, MyVector * aim);
	void update();
	void draw();
	


private:
	double life;
	int alive;
	double posX;
	double posY;
	double posZ;
	double dx, dy, dz;
	double speed;



};

#endif