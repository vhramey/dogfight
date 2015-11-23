
#ifndef _POINT_H
#define _POINT_H

class point3d
{
public:
	point3d();
	point3d(double, double, double);
	void init(double, double, double);
	inline double getX() { return x; }
	inline double getY() { return y; }
	inline double getZ() { return z; }

	double x;
	double y;
	double z;

};


#endif