#include "Quaternion.h"
#include <math.h>
#include <stdio.h>


QUATERNION::QUATERNION(void)
	{ Reset(); }

QUATERNION::QUATERNION(const QUATERNION &copyFrom)
	{ Set(copyFrom); }

QUATERNION::QUATERNION(float xT, float yT, float zT)
	{ Set(xT, yT, zT); }

QUATERNION::QUATERNION(float angle, float xT, float yT, float zT)
	{ Set(angle, xT, yT, zT); }

QUATERNION &QUATERNION::Reset(void)
	{
	q[0] = 1.0f;
	q[1] = 0.0f;
	q[2] = 0.0f;
	q[3] = 0.0f;

	return *this;
	}

QUATERNION &QUATERNION::Set(const QUATERNION &copyFrom)
	{
	q[0] = copyFrom.q[0];
	q[1] = copyFrom.q[1];
	q[2] = copyFrom.q[2];
	q[3] = copyFrom.q[3];
	
	return *this;
	}

QUATERNION &QUATERNION::Set(float xT, float yT, float zT)
	{
	QUATERNION xQ(xT, 1.0f, 0.0f, 0.0f);
	QUATERNION yQ(yT, 0.0f, 1.0f, 0.0f);
	QUATERNION zQ(zT, 0.0f, 0.0f, 1.0f);

	Set(xQ);
	PostMult(yQ);
	PostMult(zQ);
	
	return *this;
	}

QUATERNION &QUATERNION::Set(float angle, float xT, float yT, float zT)
	{
	float factor = xT*xT+yT*yT+zT*zT;
	if(!factor)
		factor = (float) EPSILON;

	float scaleBy = (float) (1.0/sqrt(factor));

	q[0] = (float) cos(angle/2.0f);

	float sinHalfAngle = (float) sin(angle/2.0f);
	q[1] = xT*scaleBy*sinHalfAngle;
	q[2] = yT*scaleBy*sinHalfAngle;
	q[3] = zT*scaleBy*sinHalfAngle;

	return *this;
	}

QUATERNION &QUATERNION::PostMult(const QUATERNION &quat)
	{
	QUATERNION tempQ(*this);
	MultAndSet(tempQ, quat);

	return *this;
	}

QUATERNION &QUATERNION::MultAndSet(const QUATERNION &quat1, const QUATERNION &quat2)
{
	q[0] = quat2.q[0]*quat1.q[0]
		   -quat2.q[1]*quat1.q[1]
		   -quat2.q[2]*quat1.q[2]
		   -quat2.q[3]*quat1.q[3];

	q[1] = quat2.q[0]*quat1.q[1]
		   +quat2.q[1]*quat1.q[0]
		   +quat2.q[2]*quat1.q[3]
		   -quat2.q[3]*quat1.q[2];

	q[2] = quat2.q[0]*quat1.q[2]
		   -quat2.q[1]*quat1.q[3]
		   +quat2.q[2]*quat1.q[0]
		   +quat2.q[3]*quat1.q[1];

	q[3] = quat2.q[0]*quat1.q[3]
		   +quat2.q[1]*quat1.q[2]
		   -quat2.q[2]*quat1.q[1]
		   +quat2.q[3]*quat1.q[0];

	return *this;
	}

QUATERNION &QUATERNION::Normalize(void)
	{
	float factor = q[0]*q[0]+
				   q[1]*q[1]+
				   q[2]*q[2]+
				   q[3]*q[3];
	float scaleBy = (float) (1.0/sqrt(factor));

	q[0] = q[0]*scaleBy;
	q[1] = q[1]*scaleBy;
	q[2] = q[2]*scaleBy;
	q[3] = q[3]*scaleBy;

	return *this;
	}

QUATERNION QUATERNION::Inversed(void)
	{
	QUATERNION qT;
	
	qT.q[1] = -q[1];
	qT.q[2] = -q[2];
	qT.q[3] = -q[3];

	return qT;
	}

float QUATERNION::DotProduct(void)
	{ return x*x+y*y+z*z+w*w; }

float QUATERNION::DotProduct(QUATERNION qT)
	{ return x*qT.x+y*qT.y+z*qT.z+w*qT.w; }

float QUATERNION::GetLength(void)
	{ return (float) sqrt(x*x+y*y+z*z+w*w); }



void QUATERNION::GetAxisAngle(float &axisX, float &axisY, float &axisZ, float &rotAngle)
	{
	float lenOfVector = q[1]*q[1]+q[2]*q[2]+q[3]*q[3];

	if(lenOfVector < EPSILON)
		{
		axisX = 1.0f;
		axisY = 0.0f;
		axisZ = 0.0f;
		rotAngle = 0.0f;
		}
	else
		{
		float invLen = (float) (1.0/sqrt(lenOfVector));
		axisX = q[1]*invLen;
		axisY = q[2]*invLen;
		axisZ = q[3]*invLen;
		rotAngle = (float) (2.0f*acos(q[0]));
		}
	}

void QUATERNION::GetAxisAngle(double &axisX, double &axisY, double &axisZ, double &rotAngle)
	{
	double lenOfVector = q[1]*q[1]+q[2]*q[2]+q[3]*q[3];

	if(lenOfVector < EPSILON)
		{
		axisX = 1.0;
		axisY = 0.0;
		axisZ = 0.0;
		rotAngle = 0.0;
		}
	else
		{
		double invLen = (double) (1.0/sqrt(lenOfVector));
		axisX = q[1]*invLen;
		axisY = q[2]*invLen;
		axisZ = q[3]*invLen;
		rotAngle = (double) (2.0*acos((double)q[0]));
		printf("q0 = %f\n", (double)q[0]);
		printf("acos = %f\n",acos((double)q[0]));
		}
	}

void QUATERNION::GetDirectionVector(float &xx, float &yy, float &zz)
	{
	Normalize();

	xx = 2.0f*(x*z-w*y);
	yy = 2.0f*(y*z+w*x);
	zz = 1.0f-2.0f*(x*x+y*y);
	}



void QUATERNION::Mult(QUATERNION a, QUATERNION b)
	{
	x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
	y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z;
	z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
	w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	}

void QUATERNION::GetMatrix(float *mT)
	{
	Normalize();

	float xx = x*x;
	float yy = y*y;
	float zz = z*z;


	mT[0] = 1.0f-2.0f*(yy+zz);
	mT[1] = 2.0f*(x*y+w*z);
	mT[2] = 2.0f*(x*z-w*y);
	mT[3] = 0.0f;

	mT[4] = 2.0f*(x*y-w*z);
	mT[5] = 1.0f-2.0f*(xx+zz);
	mT[6] = 2.0f*(y*z+w*x);
	mT[7]= 0.0f;

	mT[8] = 2.0f*(x*z+w*y);
	mT[9] = 2.0f*(y*z-w*x);
	mT[10] = 1.0f-2.0f*(xx+yy);
	mT[11] = 0.0f;

	mT[12] = 0.0f;
	mT[13] = 0.0f;
	mT[14] = 0.0f;
	mT[15] = 1.0f;
	}
