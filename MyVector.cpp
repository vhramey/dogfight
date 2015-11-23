/*
 *  Vector.cpp
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Fri Feb 07 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "MyVector.h"

MyVector::MyVector( double px, double py, double pz )
{
    X = px;
    Y = py;
    Z = pz;

    calcMag();
    UnitizePresMag();
}

// for intializing in runtime
void MyVector::init( double px, double py, double pz )
{   
    X = px;
    Y = py;
    Z = pz;

    calcMag();
    UnitizePresMag();
}

void MyVector::setVector(MyVector *A)
{
	X = A->getX();
	Y = A->getY();
	Z = A->getZ();
	mag = A->getMag();

}

// unitize and set magnitude to 1.0
void MyVector::Unitize()
{
    if( mag != 0.0 )
    {
        X = X/mag;
        Y = Y/mag;
        Z = Z/mag;
    }
    mag = 1.0;
}

// preserve magnitude when unitizing
void MyVector::UnitizePresMag()
{
	if(mag<.000001 && mag>-.000001)
		return;
    if( mag != 0.0 )
    {
        X = X/mag;
        Y = Y/mag;
        Z = Z/mag;
    }
}

// multiply unit vector by a scalar
// only changes magnitude
void MyVector::ScalarMult( double scalar )
{
    mag = mag*scalar;
	if(mag<0)
	{
		mag *= -1;
		X *= -1;
		Y *= -1;
		Z *= -1;
	}
}





// multiplies and adds components (A¥B)
double DotProduct( MyVector *A, MyVector *B )
{
	/*
	if( A->getMag() < .00001 && A->getMag() > -.00001 ||
    B->getMag() < .00001 && B->getMag() > -.00001 )
    {
        return 0.0;
    }
	double Ax, Ay, Az, Amag, Bx, By, Bz, Bmag;
	Amag = A->getMag();
	Bmag = B->getMag();
	if(Amag<0)
		Amag*=-1;
	if(Bmag<0)
		Bmag*=-1;
	
	Ax = A->getX() * Amag;
	Bx = B->getX() * Bmag;
	Ay = A->getY() * Amag;
	By = B->getY() * Bmag;
	Az = A->getZ() * Amag;
	Bz = B->getZ() * Bmag;

	double scalar =  Ax*Bx + Ay*By + Az*Bz;
	if( scalar < .00001 && scalar > -.00001 )
        scalar = 0.0;
	cout<<endl<<endl<<"dot product returning : "<<scalar<<endl;
	return scalar;
	//*/
	//*
    if( A->getMag() < .00001 && A->getMag() > -.00001 ||
    B->getMag() < .00001 && B->getMag() > -.00001 )
    {
        return 0.0;
    }
    
    double scalar;
    scalar = A->getX() * B->getX() + A->getY() * B->getY()
                + A->getZ() * B->getZ();
    if( scalar < .00001 && scalar > -.00001 )
        scalar = 0.0;
    return scalar;
	//*/
}



MyVector Add( MyVector *A, MyVector *B )
{
	double Ax, Ay, Az, Amag, Bx, By, Bz, Bmag;
	Amag = A->getMag();
	Bmag = B->getMag();
	Ax = A->getX() * Amag;
	Bx = B->getX() * Bmag;
	Ay = A->getY() * Amag;
	By = B->getY() * Bmag;
	Az = A->getZ() * Amag;
	Bz = B->getZ() * Bmag;
	
    MyVector newVector(Ax + Bx, Ay + By, Az + Bz);
    return newVector;
}



ostream &operator<<(ostream &out, MyVector &v)
{
    out<<"x = "<<v.getX()<<"  y = "<<v.getY()<<"  z = "
       <<v.getZ()<<"  mag = "<<v.getMag()<<endl;
       
    return out;
}