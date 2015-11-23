/*
 *  Vector.h
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Fri Feb 07 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

class MyVector
{
    public:
        MyVector( double px, double py, double pz );
        void init( double px, double py, double pz );
		void setVector(MyVector *A);
        void Unitize();
        void UnitizePresMag();
        void ScalarMult( double scalar );
		inline double getX(){ return X; }
        inline double getY(){ return Y; }
        inline double getZ(){ return Z; }
        inline double getMag(){return mag; }
        inline void setX( double px ) { X = px; }
        inline void setY( double py ) { Y = py; }
        inline void setZ( double pz ) { Z = pz; }
        inline void setMag( double pmag ) { mag = pmag; }
        inline void calcMag() { mag = sqrt(X*X + Y*Y + Z*Z); }
    
    private:
        double X;
        double Y;
        double Z;
        double mag;
};
    
    double DotProduct( MyVector *A, MyVector *B );
    MyVector Add( MyVector *A, MyVector *B );
    ostream &operator<<(ostream &, MyVector &v);
    
#endif
