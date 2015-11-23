#include "GenePool.h"
#include <GLUT/glut.h>
#include <algorithm>
#include <vector>
#include <time.h>
//#include <iostream>
#include <string>
//#include <fstream>

using namespace std;

int		MUTATION_RATE = 5;
#define MAX_MUTATION	1
int		TOURNAMENT_SIZE	 = 0;
int		elitism = 		0; // keep as an even number
#define CROSSOVER_RATE	70
int		NUM_TARGETS = 5;
#define TARG_X			rand()%200-100
#define TARG_Y			rand()%40-20
#define TARG_Z			rand()%200-100

#define INC_TIME		100

GenePool::GenePool(int *numPlanes)
{

  cout<<endl<<"input number of planes: ";
  cin>>*numPlanes;

  cout<<endl<<"input number of targets: ";
  cin>>NUM_TARGETS;

  cout<<"input mutation rate (5 = 5%): ";
  cin>>MUTATION_RATE;

  cout<<"input tournament size: ";
  cin>>TOURNAMENT_SIZE;

  cout<<"input elitism size: ";
  cin>>elitism;

  int i;
  for( i = 0; i < NUM_TARGETS; i++)
  {
    targets.push_back(new Plane());
  }
  time_t t;
  srand((unsigned)time(&t));

  for( i = 0; i < NUM_TARGETS; i++)
  {
	targets[i]->setX(TARG_X);
	targets[i]->setY(TARG_Y);
	targets[i]->setZ(TARG_Z);
	targets[i]->setYaw(double(rand()%1000)/1000 * 2*PI);
	targets[i]->updateQM();

	waypoints.push_back(new point3d( TARG_X, TARG_Y, TARG_Z ) );
	increments.push_back(new point3d( 0, 0, 0 ) );

	setInc(i);
    
  }
	
  // read name of weights file in from par.ini
  infile.open("par.ini");
  if( !infile )
	printf( "Error opening file");
  char filename[256];
  infile.getline(filename, 256);
  
  infile.close();

  // if the weights file name is the string "null"
  // then don't try to open it
  //if( strcmp(filename, "null") != 0 )
  //{
  cout<<"params = "<<filename<<endl;
  ifstream weights(filename);
  //}
 
  // if a file has been opened successfully with weights then
  // the planes are loaded with the appropriate weights.
  // Otherwise the planes simply have a random brain set.
  if( !weights )
  {
	printf( "Error opening file/or file 'null'" );
	for( i = 0; i < *numPlanes; i++)
	{
		planes.push_back(new Plane() );
		planes[i]->scatterbrain();
	}
  }
  else
  {
	for( i = 0; i < *numPlanes; i++)
	{
		planes.push_back(new Plane() );
		weights>>*(planes[i]);
	}
	weights.close();
  }

  numBases = planes[0]->getbrain().size();
}

GenePool::~GenePool()
{
  for(int i = 0; i < planes.size(); i++)
  {
    delete planes[i];
  }
  planes.clear();
}

void GenePool::setInc( int i )
{
  if( fabs( targets[i]->getX() - waypoints[i]->x ) < 1 )
  {
    waypoints[i]->x = TARG_X;
	waypoints[i]->y = TARG_Y;
	waypoints[i]->z = TARG_Z;
  }

  increments[i]->x = ( waypoints[i]->x - targets[i]->getX() ) / INC_TIME;
  increments[i]->y = ( waypoints[i]->y - targets[i]->getY() ) / INC_TIME;
  increments[i]->z = ( waypoints[i]->z - targets[i]->getZ() ) / INC_TIME;
}

void GenePool::UpdatePopulation()
{
  //cout<<"into updatePopulation()"<<endl;
  
  SetTargets();
  
  //cout<<"set targets..."<<endl;
  
  for(int i = 0; i < planes.size(); i++)
  {
    planes[i]->update();
	
	//printf("thrustAccel in UpdatePop = %f\n", planes[i]->getThrustAccel() );
  }
}

void GenePool::DrawPopulation(unsigned int planelist, GLuint * texture)
{
  for(int i = 0; i < planes.size(); i++)
  {
    glPushMatrix();

	
    
	planes[i]->transform();
	glCallList(planelist);
	planes[i]->shoot(texture);
	
	/*glPushMatrix();
	
	glTranslatef( planes[i]->getX(), planes[i]->getY(), planes[i]->getZ() );
	
	MyVector temp = planes[i]->getTargetVec();
	glColor3f(1.0,0.0,1.0);
	glBegin(GL_LINES);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(temp.getX(), temp.getY(), temp.getZ());
	glEnd();
	glPopMatrix();

	//*/

	//glPopMatrix();  called in the shoot function
  }
  
}

void GenePool::NextGeneration()
{
  vector<Plane *> nextGeneration = planes;

  int i = 0;

  //printf("1) before last setFitness calls\n");
  for( i = 0; i < planes.size(); i++)
  {
    planes[i]->setFitness();
  }

  sort(planes.begin(), planes.end(), comp);

  //printf("2) before printing sorted fitness calls\n");
  for( i = 0; i < planes.size(); i++ )
  {
	if( i == 0 || i == planes.size() - 1 )
		printf("fitness[%d] = %f\n", i, planes[i]->getFitness() );
  }
  printf("\n------------------\n\n");

  //printf("3) before scatterbrain loop\n");
  for( i = 0; i < planes.size()-1; i++)
  {
    if(planes[i]->getFitness() == planes[i+1]->getFitness())
	{
		planes[i]->scatterbrain();
		planes[i]->addFitness(-1);
	}
  }

  //printf("4) before elitist loop\n");
  for(i = 0; i < elitism; i++)
  {
    nextGeneration[i] = planes[i];
	//nextGeneration[i]->resetFitness();
  }

  //printf("5) before tournament/reproduction loop\n");
  for(i = elitism/2; i < planes.size()/2; i++)
  {
    Plane *father = tournament();
	Plane *mother = tournament();
	reproduce(father, mother, nextGeneration[i*2], nextGeneration[i*2+1]);
  }

  //printf("6) before deleting planes loop\n");
  for(i = elitism; i < planes.size(); i++)
  {
	  delete planes[i];
	  planes[i] = NULL;
  }
  //printf("7) out of deleting planes loop\n");

  for( i = 0; i < nextGeneration.size(); i++)
  {
    nextGeneration[i]->resetFitness();
  }

  planes = nextGeneration;

  resetTargets();
}

Plane * GenePool::tournament()
{
  Plane **group = new Plane * [TOURNAMENT_SIZE];

  int i = 0;

  for( i = 0; i < TOURNAMENT_SIZE; i++)
  {
    group[i] = planes[rand()%planes.size()];
  }

  float best = -100000000;
  int best_index = 0;
  for(i = 0; i < TOURNAMENT_SIZE; i++)
  {
    if(group[i]->getFitness() > best)
	{
      best = group[i]->getFitness();
	  best_index = i;
	}
  }
  return group[best_index];
}

void GenePool::reproduce(Plane *father, Plane *mother, Plane *(&child1), Plane *(&child2))
{
  child1 = new Plane();
  child2 = new Plane();

  vector<double> father_wt = father->getbrain();
  vector<double> mother_wt = mother->getbrain();

  if(rand()%100 < CROSSOVER_RATE)
  {
	int crossover = rand()%numBases;
    int length = rand()%(numBases - crossover);

	double temp = 0;

    for(int i = crossover; i < crossover + length; i++)
	{
      temp = father_wt[i];
      father_wt[i] = mother_wt[i];
	  mother_wt[i] = temp;
	}
  }
  child1->setbrain(father_wt);
  child1->mutatebrain(MUTATION_RATE, MAX_MUTATION);

  child2->setbrain(mother_wt);
  child2->mutatebrain(MUTATION_RATE, MAX_MUTATION);
}

void GenePool::initPlanes()
{
  for(int i = 0; i < planes.size(); i++)
  {
	if(planes[i] == NULL)
	{
		printf("9) planes[%d] == NULL!!!\n");
		planes[i] = new Plane();
	}
	else
		planes[i]->init();
  }
}

void GenePool::SetTargets()
{
  for(int i = 0; i < NUM_TARGETS; i++)
  {
    setInc(i);
	targets[i]->setX( targets[i]->getX() + increments[i]->x );
	targets[i]->setY( targets[i]->getY() + increments[i]->y );
	targets[i]->setZ( targets[i]->getZ() + increments[i]->z );
  }
  
  //cout<<"incremented targets"<<endl;

  //cout<<"planes.size() = "<<planes.size()<<endl;

  for(int i = 0; i < planes.size(); i++)
  {
	double distance = 10000;
	int target_index = 0;
	double temp = 0;
    for(int j = 0; j < NUM_TARGETS; j++)
	{
	  temp = (planes[i]->getX()-targets[j]->getX()) * (planes[i]->getX()-targets[j]->getX()) +
		  (planes[i]->getY()-targets[j]->getY()) * (planes[i]->getY()-targets[j]->getY()) +
		  (planes[i]->getZ()-targets[j]->getZ()) * (planes[i]->getZ()-targets[j]->getZ());
      if( temp < distance)
	  {
        target_index = j;
		distance = temp;
	  }
	}
    MyVector newTarget(targets[target_index]->getX() - planes[i]->getX(),
                       targets[target_index]->getY() - planes[i]->getY(),
					   targets[target_index]->getZ() - planes[i]->getZ());
	/*if(distance <= 1)
	{
		targets[target_index]->init();
		targets[target_index]->setX(TARG_X);
		targets[target_index]->setY(TARG_Y);
		targets[target_index]->setZ(TARG_Z);
		targets[target_index]->setYaw(double(rand()%1000)/1000 * 2*PI);
		targets[target_index]->updateQM();

      //targets[target_index]->init(TARG_X, TARG_Y, TARG_Z);
	  //planes[i]->addFitness(10000);
	}*/
	newTarget.calcMag();
	newTarget.Unitize();
	planes[i]->setTarget(newTarget);
	planes[i]->setDistance(distance);

	MyVector newTargeted(-targets[target_index]->getX() + planes[i]->getX(),
                         -targets[target_index]->getY() + planes[i]->getY(),
					     -targets[target_index]->getZ() + planes[i]->getZ());
	newTargeted.calcMag();
	newTargeted.Unitize();
	planes[i]->setTargeted(targets[target_index]->localize(newTargeted));
	
	//cout<<"set target and targeted for planes["<<i<<"]"<<endl;

  }
}

void GenePool::DrawTargets(unsigned int planelist)
{
  for(int i = 0; i < NUM_TARGETS; i++)
  {
    glPushMatrix();

	targets[i]->transform();
	glScalef(3,3,3);
	glColor3f(.8,1,.8);
	glCallList(planelist);

	
	//glTranslatef(targets[i]->getX(), targets[i]->getY(), targets[i]->getZ() );
	
	//glutSolidSphere(.5, 5, 5);

	glPopMatrix();
  }
  
}

void GenePool::resetTargets()
{
  for(int i = 0; i < NUM_TARGETS; i++)
  {
	  targets[i]->init();
    targets[i]->setX(TARG_X);
	targets[i]->setY(TARG_Y);
	targets[i]->setZ(TARG_Z);
	targets[i]->setYaw(double(rand()%1000)/1000 * 2*PI);
	targets[i]->updateQM();
  }
}

void GenePool::writePlanes(char *filename)
{
  ofstream file(filename);
  for(int i = 0; i < planes.size(); i++)
  {
    file<<*(planes[i]);
  }
  file.close();
}