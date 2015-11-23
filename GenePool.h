

#ifndef GENEPOOL_H
#define GENEPOOL_H


#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
//#include <GL/gl.h>
#include "Plane.h"
#include "point.h"

using namespace std;

class GenePool
{

public:
	GenePool(int *numPlanes);
	~GenePool();
	void UpdatePopulation();
	void DrawPopulation(unsigned int, GLuint *);
	void NextGeneration();
	void reproduce(Plane *father, Plane *mother, Plane *&, Plane *&);
	Plane* tournament();
    inline Plane* getPlane(int i){ return planes[i]; }
	void initPlanes();
	void DrawTargets(unsigned int);
	void SetTargets();
	void resetTargets();
	void writePlanes(char *);


	
private:

	vector<Plane*> planes;  //a vector of pointers to planes
	vector<Plane *> targets;
	int eratime;
	int eralength;
	int generation;
	int population;
	double mutationrate;
	double crossoverrate;
	double totalfitness;
	ifstream infile;

    vector<point3d *> waypoints;
	vector<point3d *> increments;

	void setInc(int);

    int numBases;

};

#endif