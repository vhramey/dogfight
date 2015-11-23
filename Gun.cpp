#include "Gun.h"

Gun::Gun()
{
	life = 0;
	alive = 0;
	posX = 0;
	posY = 0;
	posZ = 0;
	speed = 2;
	dx = 0;
	dy = 0;
	dz = 0;
}



void Gun::fire(double x, double y, double z, MyVector * aim)
{
	posX = x;
	posY = y;
	posZ = z;
	life = 1;
	alive = 1;
	dx = aim->getX() * speed;
	dy = aim->getY() * speed;
	dz = aim->getZ() * speed;

}

void Gun::update()
{
	if(!alive)
		return;

	draw();

	life -= .02;
	if(life<0)
		alive = 0;

	//dy-= .001;   //gravity term  -- around .001

	posX += dx;
	posY += dy;
	posZ += dz;

	


}

void Gun::draw()
{
	//shitty fast bullets
	/*
	glColor3f(1,1,0);
	glBegin(GL_POINTS);
	glVertex3d(posX, posY, posZ);
	glEnd();
	//*/
	//*  //awesome bullets, but slow
	float ddx = dx*2.5;
	float ddy = dy*2.5;
	float ddz = dz*2.5;

	//glLineWidth(20);

	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBegin(GL_LINES);
		glColor4f(1,1,0,0);
		glVertex3d(posX, posY, posZ);
		glColor4f(1,1,.5,1);
		glVertex3d(posX + ddx, posY + ddy, posZ + ddz);
		
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//*/
}
