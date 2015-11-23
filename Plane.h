/*
 *  Plane.h
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Thu Feb 06 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef PLANE_H
#define PLANE_H

//* //old
#define GRAVITY 0.15
#define LIFT_CONSTANT 0.8
#define KITE_CONSTANT 0.8
#define DRAG_CONSTANT 0.8
#define FLAP_CONSTANT 3.0
#define MAX_FLAP_ANGLE 60.0  //was 60
#define MAX_YAW_FLAP_ANGLE 10.0
#define MOMENT_INERTIA_ROLL 0.05 //was .05
#define MOMENT_INERTIA_YAW 0.02
#define MOMENT_INERTIA_PITCH 0.05 //was .05
#define PITCH_CORRECT 0.2 //was .2
#define YAW_CORRECT 0.1
#define MAX_THRUST .3
#define DELTA_FLAP .025
#define BOX_SIZE 20
//#define PI 3.141592
//*/


#include "MyVector.h"
#include "Quaternion.h"
#include "NeuralNet.h"
#include "Gun.h"
#include <fstream>
#include <iostream>

using namespace std;



class Plane
{
    public:
        Plane();
        ~Plane();
		void init();
		void scatterbrain();
		void mutatebrain(int, double);
		void setbrain(vector<double> &briz);
		vector<double> getbrain() {return weights;}
		inline vector<double> getWeights() { return brain.GetWeights(); }
		inline void putWeights( vector<double> pWeights ) { brain.PutWeights( pWeights ); }
		inline int getNumberOfWeights() { return brain.GetNumberOfWeights(); }
		void think();
        void update();
        void calcAccel();
        void calcAngles();
        void calcVel();
		void calcPos();
		void updateVectors();
		void updateQM();
        void resetPerps();
        void transform();
		void capMaxMin(); // caps incremented values returned by neural net
		inline void setPrinting( double a ){ printing = (int)a; }
		inline void setThrustAccel( double pthrustAccel ){ thrustAccel = pthrustAccel;}
		inline void setfrFlap(double a){frFlap = a;}
		inline void setflFlap(double a){flFlap = a;}
		inline void setrpFlap(double a){rpFlap = a;}
		inline void setryFlap(double a){ryFlap = a;}
        inline double getPitch() { return pitch; }
        inline double getRoll() { return roll; }
        inline double getYaw() { return yaw; }
        inline void setPitch( double pPitch ) { pitch = pPitch; }
        inline void setRoll( double pRoll ) { roll = pRoll; }
        inline void setYaw( double pYaw ) { yaw = pYaw; }
		inline void setX( double px ) { posX = px; }
		inline void setY( double py ) { posY = py; }
		inline void setZ( double pz ) { posZ = pz; }
		inline double getThrustAccel(){ return thrustAccel; }
		inline double getX() { return posX; }
		inline double getY() { return posY; }
		inline double getZ() { return posZ; }
		inline MyVector getHeadingVec() { return *heading; }
        inline MyVector getPerpVecY() { return *perpY; }
        inline MyVector getPerpVecZ() { return *perpZ; }
		inline MyVector getVelVec() { return *vel; }
		inline MyVector getTargetVec() { return target; }
		inline MyVector getGlobalTargetVec() {return globalTarget; }
		void setTarget(MyVector t);
		void setTargeted(MyVector t);
		MyVector localize(MyVector t);
		inline void addFitness(double d) { fitness += d; }
		inline void resetFitness() { fitness = 0; }
		inline void setDistance(double d) { distance_to_target = d; }
		inline void shootingon() { shooting = 1; }
		inline void shootingoff(){ shooting = 0; }


		void setFitness();
		inline double getFitness() { return fitness; }

		void shoot(GLuint * texture);

        //function to define angles
        //function to normalize vectors
        //function for flaps
        //define perpendicular unit vector

    private:

		bool shooting;

		double propvision;
		double proprotation;

		int numshots;
		int shotindex;
		int shotdelay;
		Gun * shot;

		int lockcount;
		int targetedcount;

		NeuralNet brain;
		vector<double> weights;
		double fitness;

		MyVector targeted;
		
		MyVector target;
		MyVector globalTarget;
		double distance_to_target;
        
		QUATERNION rotation;
		float m16[16]; // rotation matrix
        
        // (x,y,z) acceleration vector for plane
        MyVector *netAccel;
        
        // (x,y,z) unit velocity vector for plane
        MyVector *vel;
        
        // (x,y,z) position vector for plane
        double posX, posY, posZ;
        
        // (x,y,z) unit heading vector for plane
        MyVector *heading;
        
        // axes of plane (perpendicular to eachother)
		MyVector *perpY;
        MyVector *perpZ;
        
        // delta-pitch, delta-roll, delta-yaw
        double pitch, roll, yaw;
        
        // flap angles (received from NeuralNet)
        double frFlap, flFlap, rpFlap, ryFlap;  //rpFlap = rearpitchflap, ryflap = rearyawflap
        double wflapDist, tflapDist; //distance from planes axis, wing and tail
        
        // Acceleration due to thruster (received from NeuralNet)
        double thrustAccel;
        
        // mass
        double mass;

		int lasttickcount;
		double dt;
		int printing;

};

bool comp(Plane *, Plane *);

ostream &operator<<(ostream &output, Plane &rhs);
istream &operator>>(istream &input, Plane &rhs );

#endif

