/*
 *  Plane.cpp
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Thu Feb 06 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "Plane.h"
#include "MyVector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
//#include <windows.h>
#include <iostream>

Plane::Plane():target(0,0,0), globalTarget(0,0,0), targeted(0,0,0)
{
	shooting = 0;
	fitness = 0;
	lockcount = 0;
	targetedcount = 0;
	for(int i=0; i<16; i++)
		m16[i] = 0;
	m16[0] = 1;
	m16[5] = 1;
	m16[10] = 1;
	m16[15] = 1;

	shotindex=0;
	shotdelay = 0;
	numshots=100;
	shot = new Gun[numshots];

	propvision = 1;
	proprotation = 0;

	
	//lasttickcount = GetTickCount();
	dt = 0;

    netAccel = new MyVector(0.0, 0.0, 0.0);
	//thrustAccel = MAX_THRUST / 2;
    
    vel = new MyVector(0.0, 0.0, 0.0);
    
    pitch = 0;
    roll = 0;
    yaw = 0;

	posX = 0;
	posY = 0;
	posZ = 0;
    
    heading = new MyVector(1.0,0.0,0.0);
    
    perpY = new MyVector(0.0,1.0,0.0);
    
    perpZ = new MyVector(0.0,0.0,1.0);
    
    //frFlap = 0.0;
    flFlap = 0.0;
    rpFlap = 0.0;
	ryFlap = 0.0;
    wflapDist = 2.0; // subject to change
	tflapDist = 2.0;

	mass = 170;
	printing = 0;

	//for(i=0; i<70; i++)
	//	weights.push_back((double)(rand()%2000 -1000)/1000);
	//brain.PutWeights(weights);
	weights = brain.GetWeights();


}

Plane::~Plane()
{
	weights.clear();
    delete netAccel;
    delete heading;
    delete perpY;
    delete perpZ;
    delete vel;
	delete shot;
}

void Plane::scatterbrain()
{
	weights = brain.GetWeights();
	for(int poop = 0; poop < weights.size(); poop++)
		weights[poop] = (double)(rand()%4000 -2000)/2000;
	brain.PutWeights(weights);

}

void Plane::mutatebrain(int mutationRate, double maxMutation)
{
	//cout<<"mutating brain";
	weights = brain.GetWeights();
	for(int poop = 0; poop < weights.size(); poop++)
	{
		if((rand()%100) < mutationRate )
		{
			//cout<<".";
			weights[poop] += (double)((rand()%4001 - 2000)/2000.0)*maxMutation;
		}
	}
	brain.PutWeights(weights);
	//cout<<endl;
}

void Plane::setbrain(vector<double> &briz)
{
	weights = briz;
	brain.PutWeights(weights);

}

void Plane::init()
{
	
	rotation.Reset();

	for(int i=0; i<16; i++)
		m16[i] = 0;
	m16[0] = 1;
	m16[5] = 1;
	m16[10] = 1;
	m16[15] = 1;

	
	//lasttickcount = GetTickCount();
	dt = 0;

    netAccel->init(0.0, 0.0, 0.0);
	thrustAccel = MAX_THRUST / 2 ;
    
    vel->init(0.0, 0.0, 0.0);
    
    pitch = 0;
    roll = 0;
    yaw = 0;

	posX = 0;
	posY = 0;
	posZ = 0;

	//frFlap = 0.0;
    flFlap = 0.0;
    rpFlap = 0.0;
	ryFlap = 0.0;

	setYaw(-PI);
	updateQM();
	setRoll(PI/2);
	updateQM();
	//setYaw(double(rand()%1000)/1000 * 2*PI);
	//updateQM();

}

void Plane::think()
{
	vector<double> inputs;
	vector<double> outputs;
	inputs.clear();
	outputs.clear();

	MyVector up(0,1,0);
	MyVector localup(0,0,0);
	localup = localize(up);
	double dist = distance_to_target;
	if(dist < .005)
		dist = .005;
	double d = 1.0/dist;
	if(d>1)
		d = 1;
	//*
	//inputs.push_back(heading->getX());
	//inputs.push_back(heading->getY());
	//inputs.push_back(heading->getZ());
	//inputs.push_back(perpY->getX());
	//inputs.push_back(perpY->getY());
	//inputs.push_back(perpY->getZ());
	inputs.push_back(target.getX());
	inputs.push_back(target.getY());
	inputs.push_back(target.getZ());
	inputs.push_back(localup.getX());
	inputs.push_back(localup.getY());
	inputs.push_back(localup.getZ());
	//inputs.push_back(targeted.getX());
	//inputs.push_back(targeted.getY());
	//inputs.push_back(targeted.getZ());
	inputs.push_back(d);

	//inputs.push_back(thrustAccel);

	//printf("%d", inputs.size());
	
	outputs = brain.Update(inputs);
	
	if(printing)
	{
		for(int j=0; j<5; j++)
			printf(" %f ", outputs[j]);
		printf("\n");
	}
	/*
	// Neural Net returns delta values
	thrustAccel += (outputs[0] - .5) * MAX_DELTA_THRUST;// * MAX_THRUST;
	frFlap += (outputs[1] - .5);// * 60;
    flFlap += (outputs[2] - .5);// * 60;
    rpFlap += (outputs[3] - .5);// * 60;
	ryFlap += (outputs[4] - .5);// * 10;
	capMaxMin(); // Doesn't allow values to exceed maximums/minimums
	//printf("thrustAccel after capMaxMin = %f:\n", thrustAccel);
	//*/


	// NOTE: I have removed the frFlap as an output. Instead I
	// have doubled the torque caused by the flFlap which simulates
	// the frFlap having the same angle in the opposite direction as the flFlap

	// Neural Net returns complete values
	/*
	if(thrustAccel > outputs[0]  * MAX_THRUST)
		thrustAccel -= .001;
	if(thrustAccel < outputs[0]  * MAX_THRUST)
		thrustAccel += .001;
	//*/
	//thrustAccel = outputs[0]  * MAX_THRUST;
	//*/

	thrustAccel = .25;

	// This increments the flap angles slowly toward
	// the angle specified by the neural net
	/*if(flFlap > (outputs[1] - .5) * MAX_FLAP_ANGLE )
		flFlap -= DELTA_FLAP;
	if(flFlap < (outputs[1] - .5) * MAX_FLAP_ANGLE )
		flFlap += DELTA_FLAP;
	if( rpFlap < (outputs[2] - .5) * MAX_FLAP_ANGLE )
		rpFlap += DELTA_FLAP;
	if( rpFlap > (outputs[2] - .5) * MAX_FLAP_ANGLE )
		rpFlap -= DELTA_FLAP;
	if( ryFlap > (outputs[3] - .5) * MAX_YAW_FLAP_ANGLE )
		ryFlap -= DELTA_FLAP;
	if( ryFlap < (outputs[3] - .5) * MAX_YAW_FLAP_ANGLE )
		ryFlap += DELTA_FLAP;*/

    flFlap = (outputs[0] - .5) * MAX_FLAP_ANGLE;
    rpFlap = (outputs[1] - .5) * MAX_FLAP_ANGLE;
	ryFlap = (outputs[2] - .5) * MAX_YAW_FLAP_ANGLE;

	/*if(outputs[3] > .5)
		shootingon();
	else
		shootingoff();*/

	if(DotProduct(&globalTarget, heading) > .999)
	{
      shootingon();
	}
	else
	{
	  shootingoff();
	}

	//for now, no shooting at all
	//shootingoff();
	//*/

	/*
	thrustAccel =.5 * MAX_THRUST;
	frFlap = (.5 - .5) * 90;
    flFlap = (.5 - .5) * 90;
    rpFlap = (.6 - .5) * 90;
	ryFlap = (.5 - .5) * 90;
	//*/

}

void Plane::resetPerps()
{
    perpY->init(0.0, 1.0, 0.0);
    perpZ->init(0.0, 0.0, 1.0);
}

void Plane::update()
{
	//int ticks = GetTickCount();
	//dt = (double)(ticks - lasttickcount)/100;
	//lasttickcount = ticks;

	//printf("%f", dt);

	dt = 1;

	think();

	updateVectors();
    calcAccel();
    calcVel();
	calcPos();
	calcAngles();
	updateQM();

	setFitness();
}



void Plane::calcVel()
{
    //vel = Add( netAccel, vel ) ;//need a fucking = operator
	double velmag = vel->getMag();
	double accmag = netAccel->getMag();
	//vel->setMag(vel->getMag() + netAccel->getMag());
	vel->setX(vel->getX()*velmag + netAccel->getX()*accmag);
	vel->setY(vel->getY()*velmag + netAccel->getY()*accmag);
	vel->setZ(vel->getZ()*velmag + netAccel->getZ()*accmag);
	vel->calcMag();
	vel->UnitizePresMag();
}

void Plane::calcAccel()
{
    // reset acceleration components
    netAccel->setX(0);
    netAccel->setY(0);
    netAccel->setZ(0);
	netAccel->setMag(0);
    
    // adjust accel based on thruster acceleration
    // - in direction of heading
	//printf("thrustAccel in calcAccel = %f\n:", thrustAccel);
    netAccel->setX(thrustAccel * heading->getX() * dt );
    netAccel->setY(thrustAccel * heading->getY() * dt );
    netAccel->setZ(thrustAccel * heading->getZ() * dt );
    
    // adjust accel based on weight
    netAccel->setY( netAccel->getY() - GRAVITY  * dt);
    
    // adjust accel based on drag 
    // - proportional to velocity, in opposite direction
    netAccel->setX( netAccel->getX() - DRAG_CONSTANT * vel->getX() * vel->getMag() * dt );
    netAccel->setY( netAccel->getY() - DRAG_CONSTANT * vel->getY() * vel->getMag() * dt );
    netAccel->setZ( netAccel->getZ() - DRAG_CONSTANT * vel->getZ() * vel->getMag() * dt );
    
    // adjust accel based on lift
    // - base accel components perpendicular vector
    // - liftMag is proportional to velMag
    double liftMag = LIFT_CONSTANT * vel->getMag() * fabs(DotProduct(vel, heading) * dt);
	liftMag -= KITE_CONSTANT * vel->getMag() * DotProduct(vel, perpY);
    // zero when plane is horizontal
    netAccel->setX( netAccel->getX() + perpY->getX() * liftMag );
    // max when plane is horizontal
    netAccel->setY( netAccel->getY() + perpY->getY() * liftMag );
    // zero when plane is horizontal
    netAccel->setZ( netAccel->getZ() + perpY->getZ() * liftMag );

	

	netAccel->calcMag();
	netAccel->UnitizePresMag();
    
}

// Left wing is at a negative distance
// Right wing is at a positive distance
// Positive torque in counterclockwise direction
// Positive rotation in counterclockwise direction
void Plane::calcAngles()
{
    // perpendicular force components on flaps
    double fPerpFL, fPerpFR, fPerpRP, fPerpRY;
    // Y force components on flaps
    double fy_FL, fy_FR, fy_RP;
    // X force components on flaps
    double fx_FL, fx_FR, fx_RP;
	double fz_RY;
    double torque;
    
    
	//NOTE: I have cut out calculations involving the frFlap and instead
	// have doubled the torque caused by the flFlap which simulates
	// the frFlap having the same angle in the opposite direction as the flFlap

    // Calculate the Roll
    // vel¥heading is the projection of velocity on heading unit vectors
    fPerpFL = vel->getMag() * DotProduct(vel, heading) * FLAP_CONSTANT *
                sin( -flFlap*PIOVER180 );
    /*fPerpFR = vel->getMag() * DotProduct(vel, heading) * FLAP_CONSTANT *
                sin( -frFlap*PIOVER180 );*/
    fy_FL = fPerpFL * cos( flFlap*PIOVER180 );
    //fy_FR = fPerpFR * cos( frFlap*PIOVER180 );
    
    torque = 2* wflapDist * ( /*fy_FR + */(-fy_FL) );

    //******** delta-roll **********//
    roll += MOMENT_INERTIA_ROLL * torque;
    
    // Calculate the Yaw 
    //fx_FL = abs( fPerpFL * sin( flFlap*PIOVER180 ) );
    //fx_FR = abs( fPerpFR * sin( frFlap*PIOVER180 ) );
	fPerpRY = vel->getMag() * DotProduct(vel, heading) * FLAP_CONSTANT *
                sin( -ryFlap*PIOVER180 );
	fz_RY= fPerpRY * cos( ryFlap*PIOVER180 );
    torque = /*wflapDist * (fx_FL - fx_FR) +*/ tflapDist * fz_RY;
    
    //******** delta-yaw *********//
    yaw += MOMENT_INERTIA_YAW * torque;
        
    // Calculate the Pitch
    fPerpRP = vel->getMag() * DotProduct(vel, heading) * FLAP_CONSTANT *
                sin( -rpFlap*PIOVER180 );
    fy_RP = fPerpRP * cos( rpFlap*PIOVER180 );

	torque = tflapDist * fy_RP;
    
    //****** delta-pitch *********//
    pitch -= MOMENT_INERTIA_PITCH * torque;


	//addin pitch and yaw "straightening"
	double yc = DotProduct(vel, perpZ) * vel->getMag() * YAW_CORRECT;
	yaw -= yc;

	double pc = DotProduct(vel, perpY) * vel->getMag() * PITCH_CORRECT;
	pitch += pc;

}

void Plane::calcPos()
{
	double mag = vel->getMag();
	posX += vel->getX() * mag;
	posY += vel->getY() * mag;
	posZ += vel->getZ() * mag;
}


void Plane::updateVectors()
{
	float xx, yy, zz;
	/*
	// initialize new heading
	rotation.GetDirectionVector(xx, yy, zz);
	heading->init( xx, yy, zz );

	QUATERNION temprot(rotation);
	// initialize new perpY
	temprot.PostMult(QUATERNION(PI/2, 0, 0));
	temprot.GetDirectionVector(xx, yy, zz);
	perpY->init(xx, yy, zz);
	temprot.Set(rotation);

	// initialize new perpZ
	temprot.PostMult(QUATERNION(0, PI/2, 0));
	temprot.GetDirectionVector(xx, yy, zz);
	perpZ->init(xx,yy,zz);
	//*/
	//*
	//all these are adjusted for planyness
	// initialize new heading
	rotation.GetDirectionVector(xx, yy, zz);
	perpY->init( xx, yy, zz );

	QUATERNION temprot(rotation);
	// initialize new perpY
	temprot.PostMult(QUATERNION(PI/2, 0, 0));
	temprot.GetDirectionVector(xx, yy, zz);
	perpZ->init(xx, yy, zz);
	temprot.Set(rotation);

	// initialize new perpZ
	temprot.PostMult(QUATERNION(0, PI/2, 0));
	temprot.GetDirectionVector(xx, yy, zz);
	heading->init(xx,yy,zz);	
	//*/
}

void Plane::updateQM()
{
	// calculates vectors globally
	// updates the planes rotation quat
	//QUATERNION increment(pitch, -yaw, roll);
	
	float rroll = roll;
	float ppitch = pitch;
	float yyaw = yaw;

	if(rroll<.0005 && rroll>-.0005)
		rroll = 0;
	if(ppitch<.0005 && ppitch>-.0005)
		ppitch = 0;
	if(yyaw<.0005 && yyaw>-.0005)
		yyaw = 0;

	QUATERNION increment(-rroll, -ppitch, -yyaw);//adjusted
	rotation.PostMult(increment);

	
	glPushMatrix();
	glLoadMatrixf(m16);
	

	float ax, ay, az, arot;
	increment.GetAxisAngle(ax, ay, az, arot);
	float rot = -arot*PIUNDER180;
	
	glRotatef(rot, ax, ay, az);

	// normalize matrix to correct for drift
	/*
	float sum = 0;
	for(int i=0; i<16; i++)
	{
		sum += m16[i]*m16[i];
	}
	if(sum<4)
		glScalef(1.001, 1.001, 1.001);
	if(sum>4)
		glScalef(.999, .999, .999);
	//*/

	glGetFloatv(GL_MODELVIEW_MATRIX, m16);

	glPopMatrix();

	pitch = roll = yaw = 0;

	
}

void Plane::transform()
{
	/*  //wrapping
	if(posX>BOX_SIZE)
		posX = -BOX_SIZE;
	else if(posX<-BOX_SIZE)
		posX = BOX_SIZE;
	if(posY>BOX_SIZE)
		posY = -BOX_SIZE;
	else if(posY<-BOX_SIZE)
		posY = BOX_SIZE;
	if(posZ>BOX_SIZE)
		posZ = -BOX_SIZE;
	else if(posZ<-BOX_SIZE)
		posZ = BOX_SIZE;
	//*/

	glTranslatef(posX, posY, posZ);
	
	//printf("pos =  %f %f %f  ", posX, posY, posZ);
	//printf("vel =  %f\n", vel->getMag());
	
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	
	/*glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(heading->getX(),heading->getY(),heading->getZ());
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(perpY->getX(),perpY->getY(),perpY->getZ());
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(perpZ->getX(),perpZ->getY(),perpZ->getZ());
	glColor3f(1,1,1);
	glVertex3f(0,0,0);
	glVertex3f(netAccel->getX(),netAccel->getY(),netAccel->getZ());
	glColor3f(0,1,1);
	glVertex3f(0,0,0);
	glVertex3f(vel->getX(),vel->getY(),vel->getZ());
	*/
	glColor3f(1,1,0);
	glVertex3f(0,0,0);
	glVertex3f(globalTarget.getX(), globalTarget.getY() , globalTarget.getZ() );
	
	
	
	glEnd();
	glEnable(GL_LIGHTING);
	//*/
	glMultMatrixf(m16);

	glColor3f(.4,.4,.4);

	if(lockcount)
		glColor3f(1,0,0);

	if(targetedcount > 0 )
		glColor3f(0,0,1);
	if(targetedcount < 0 )
		glColor3f(0,1,0);

	//for(int i = 0; i<4; i++)
	//	printf("%f %f %f %f\n", m16[0+i], m16[4+i], m16[8+i], m16[12+i]);

}

void Plane::capMaxMin()
{
	if(thrustAccel < 0.0 )
		thrustAccel = 0.0;
	if(thrustAccel > MAX_THRUST)
		thrustAccel = MAX_THRUST;
	
	if(flFlap < -MAX_FLAP_ANGLE)
		flFlap = -MAX_FLAP_ANGLE;
	if(flFlap > MAX_FLAP_ANGLE)
		flFlap = MAX_FLAP_ANGLE;
	if(rpFlap < -MAX_FLAP_ANGLE)
		rpFlap = -MAX_FLAP_ANGLE;
	if(rpFlap > MAX_FLAP_ANGLE)
		rpFlap = MAX_FLAP_ANGLE;
	if(ryFlap < -MAX_YAW_FLAP_ANGLE)
		ryFlap = -MAX_YAW_FLAP_ANGLE;
	if(ryFlap > MAX_YAW_FLAP_ANGLE)
		ryFlap = MAX_YAW_FLAP_ANGLE;
}

void Plane::setTarget( MyVector t )
{
	//t.init(0,1,0);
	target = t;
	globalTarget = t;

	globalTarget.calcMag();
	globalTarget.Unitize();

	target = localize(t);

	

}

void Plane::setTargeted( MyVector t )
{
	//t.init(0,1,0);
	targeted = t;
	
}

void Plane::setFitness()
{
  //MyVector vecTowards(0,0,1);
  MyVector vecUp(0,1,0);
  
  double coeff0 =  50; // up coeff
  double coeff1 = .5; // heading coeff
  double coeff2 = .01; // vel coeff
  double scalar;

  // account for velocity vector aligning with target
  scalar = DotProduct(&globalTarget, vel);
  //if( scalar < -.5 )
	//	coeff2 *= 10;
  double abscalar = scalar;
  if(scalar<0)
	  abscalar *= -2.0;
  fitness += coeff2*scalar*abscalar; // fitness proportional to x

  // account for heading vector
  scalar = DotProduct(&globalTarget, heading);
  abscalar = scalar;
  if(scalar<0)
	  abscalar *= -2.0;
  fitness += coeff1*scalar*abscalar; // fitness proportional to x
  if(scalar > .999)
	  lockcount++;
  else
	  lockcount = 0;
  if(lockcount)
	  fitness += coeff1*scalar*lockcount;

  /*MyVector  B(-1,0,0);

  scalar = DotProduct(&targeted, &B);
  abscalar = scalar;
  fitness -= coeff0*scalar; // fitness proportional to x
  if(scalar > .2)
	  targetedcount++;
  else if(scalar < -.2)
	  targetedcount--;
  else
	  targetedcount=0;
  if(targetedcount)
	  fitness -= coeff0*scalar*targetedcount;*/

  /*
  double dist = distance_to_target*distance_to_target;
  if(dist<1)
	  dist = 1;
  if( lockcount)//scalar > 0.9 ) // 
	fitness += 1.0 / dist;
  //*/

  // account for up vector
  
  scalar = DotProduct(&vecUp, perpY);
  if( scalar < 0)
  {
	 //coeff0 *= 10;
     fitness += coeff0*scalar; // fitness proportional to x
  }
  //*/
  if( posY < -50 )
  fitness += posY;
  //*/
}

bool comp(Plane *p1, Plane *p2)
{
  return (p1->getFitness() > p2->getFitness());
}

void Plane::shoot(GLuint * texture)
{
	if(shooting)
	{
		shotdelay++;

		if(shotdelay>3)
		{
			shotdelay = 0;

			double lx, ly, lz;
			double rx, ry, rz;
			double radius = .35;
			double forward = .25;
			double down = .05;
			double fsize = .1;

			lx = posX - perpZ->getX()*radius + heading->getX()*forward - perpY->getX()*down;
			ly = posY - perpZ->getY()*radius + heading->getY()*forward - perpY->getY()*down;
			lz = posZ - perpZ->getZ()*radius + heading->getZ()*forward - perpY->getZ()*down;

			rx = posX + perpZ->getX()*radius + heading->getX()*forward - perpY->getX()*down;
			ry = posY + perpZ->getY()*radius + heading->getY()*forward - perpY->getY()*down;
			rz = posZ + perpZ->getZ()*radius + heading->getZ()*forward - perpY->getZ()*down;

			shot[shotindex].fire(lx, ly, lz, heading);
			shotindex++;
			if(shotindex==numshots)
				shotindex=0;

			shot[shotindex].fire(rx, ry, rz, heading);
			shotindex++;
			if(shotindex==numshots)
				shotindex=0;
			
			glDisable(GL_LIGHTING);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);

			glColor3f(1,1,1);

			glPushMatrix();

			glTranslated(-forward,-radius,-down);

			glBindTexture(GL_TEXTURE_2D, texture[5]);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3d(0,-fsize,  -fsize);
			glTexCoord2f(1,0);
			glVertex3d(0,fsize, -fsize);
			glTexCoord2f(1,1);
			glVertex3d(0,fsize, fsize);
			glTexCoord2f(0,1);
			glVertex3d(0,-fsize, fsize);
			glEnd();

			glPopMatrix();

			glPushMatrix();

			glTranslated(-forward,radius,-down);

			

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3d(0,-fsize,  -fsize);
			glTexCoord2f(1,0);
			glVertex3d(0,fsize, -fsize);
			glTexCoord2f(1,1);
			glVertex3d(0,fsize, fsize);
			glTexCoord2f(0,1);
			glVertex3d(0,-fsize, fsize);
			glEnd();

			glPopMatrix();

			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDepthMask(GL_TRUE);
		}

	}

	//draw prop

	double f = .46;
	double d = .05;
	double psize = .2;

	proprotation += 2;

	if(propvision == 1)
		propvision = .5;
	else
		propvision = 1;
	/*
	else if(propvision == 1.01)
		propvision = .99;
	else if(propvision == .99)
		propvision = 1;
	//*/
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glColor4d(1,1,1, propvision );

	glPushMatrix();

	glTranslated(-f,0,-d);
	glRotated(proprotation, 1, 0, 0);

	glBindTexture(GL_TEXTURE_2D, texture[4]);

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3d(0,-psize,  -psize);
	glTexCoord2f(1,0);
	glVertex3d(0,psize, -psize);
	glTexCoord2f(1,1);
	glVertex3d(0,psize, psize);
	glTexCoord2f(0,1);
	glVertex3d(0,-psize, psize);
	glEnd();

	glPopMatrix();
	
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);


	glPopMatrix();

	int i;
	for(i=0; i<numshots; i++)
		shot[i].update();

}

MyVector Plane::localize( MyVector t )
{
	//t.init(0,1,0);
	MyVector temp(0,0,0);

	double x = t.getX();
	double y = t.getY();
	double z = t.getZ();
	
	/*
	target.setX( m16[0]*x + m16[4]*y + m16[8 ]*z + m16[12] );
	target.setY( m16[1]*x + m16[5]*y + m16[9 ]*z + m16[13] );
	target.setZ( m16[2]*x + m16[6]*y + m16[10]*z + m16[14] );
	//*/

	temp.setX( m16[0]*x + m16[1]*y + m16[2 ]*z + m16[3 ] );
	temp.setY( m16[4]*x + m16[5]*y + m16[6 ]*z + m16[7 ] );
	temp.setZ( m16[8]*x + m16[9]*y + m16[10]*z + m16[11] );

	temp.calcMag();
	temp.Unitize();

	return temp;
}

ostream &operator<<( ostream &output, Plane& rhs)
{
	vector<double> weights = rhs.getWeights();
	for( int i = 0; i < weights.size(); i++ )
	{
		output << weights[i] << endl;
	}
	return output;
}

istream &operator>>( istream &input, Plane& rhs )
{
	vector<double> weights;
	double weight;
	for( int i = 0; i < rhs.getNumberOfWeights(); i++ )
	{
		input >> weight;
		weights.push_back(weight);
	}
	rhs.setbrain(weights); // use this until we resolve duplicity issues
	//rhs.putWeights( weights );

	return input;
}

