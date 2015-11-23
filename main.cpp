//testing the rotation and shit

//#include <windows.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <memory.h>
#include <assert.h>

#include "Displaylists.h"
#include "Plane.h"
#include "GenePool.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

int GENERATION_TIME	=	2000;
#define RESET_TIME			1000000

using namespace std;
extern GLuint Gen3DObjectList(void);
GLUquadricObj * sphere;
GLuint texture[6];  
GLuint planelist;

int *dogs = new int[1]; // keep as even number
int myCount=0;
int countCam = 0;
int generationCount = 0;
int chaseplane = 0;
int camera = 3;
int saveInc = 10;
int saveCount = 0;

GenePool *Genomes;

//vector<Plane *> test = Genomes.getPopulation();

Plane *test = NULL;

//vector<double> bestsofar= test[0].getbrain();
int bested=0;


float thrust = .11;

float zoom = -20;
float xrot=0, zrot=0;

int keys[256];

void init();

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
} TextureImagek;												// Structure Name



bool LoadTGA(char *filename, int p)			// Loads A TGA File Into Memory
{    
	TextureImagek *texturek;
	texturek = new TextureImagek;
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");						// Open The TGA File
	
	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*
			return false;									// Return False
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			return false;									// Return False
		}
	}

	texturek->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texturek->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texturek->width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texturek->height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texturek->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texturek->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texturek->width*texturek->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texturek->imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texturek->imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texturek->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texturek->imageData!=NULL)						// Was Image Data Loaded
			free(texturek->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texturek->imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texturek->imageData[i] = texturek->imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texturek->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);											// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texture[p]);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture[p]);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texturek[0].bpp==24)									// Was The TGA 24 Bits
	{
		type=GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, type, texturek[0].width, texturek[0].height, 0, type, GL_UNSIGNED_BYTE, texturek[0].imageData);
	gluBuild2DMipmaps(GL_TEXTURE_2D, type, texturek[0].width, texturek[0].height, type, GL_UNSIGNED_BYTE, texturek[0].imageData);

	return true;											// Texture Building Went Ok, Return True
}





void drawplain()
{
	float size = 50;//BOX_SIZE/2;
	float ssize = 2000;
	
	glPushMatrix();
	//if(a)
	test = Genomes->getPlane(chaseplane);
	double x = test->getX();
	double y = test->getY();
	double z = test->getZ();
	glTranslatef(x, 0, z );

	float speedfactor = 4;  //.5 = realistic, when scale=1
	float scale = 8;
	float s = (x)/(ssize) * speedfactor;
	float t = (z)/(ssize) * speedfactor;

	//glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	gluQuadricOrientation(sphere, GLU_INSIDE);
	gluQuadricTexture(sphere, GL_TRUE);

	glPushMatrix();
	//glTranslatef(0,y,0);
	glRotatef(-90, 1,0,0);
	//glRotatef(180, 0,0,1);
	glColor3f(1,1,1);
	gluSphere(sphere, ssize, 10, 10);

	glPopMatrix();


	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glBegin(GL_QUADS);    ////land

	glColor3f(1.0, 1.0, 1.0 );
	glNormal3f(0,1,0);
	glTexCoord2f(0 + s, 0 + t);
	glVertex3f(-ssize, -size, -ssize);
	glTexCoord2f(0 + s, scale + t);
	glVertex3f(-ssize, -size, ssize);
	glTexCoord2f(scale + s, scale + t);
	glVertex3f(ssize, -size, ssize);
	glTexCoord2f(scale + s, 0 + t);
	glVertex3f(ssize, -size, -ssize);
	glEnd();


	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindTexture(GL_TEXTURE_2D, texture[2]);

	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, .5 );
	glNormal3f(0,-1,0);
	glTexCoord2f(0 + s, 0 + t);
	glVertex3f(-ssize, size+2, -ssize);
	glTexCoord2f(1 + s, 0 + t);
	glVertex3f(ssize, size+2, -ssize);
	glTexCoord2f(1 + s, 1 + t);
	glVertex3f(ssize, size+2, ssize);
	glTexCoord2f(0 + s, 1 + t);
	glVertex3f(-ssize, size+2, ssize);
	glEnd();



	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
	/*
	glColor3f(0.0f, 1.0f, 1.0f );
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(size, -size, -size);
	glVertex3f(size, -size, size);
	glVertex3f(size, size, size);
	glVertex3f(size, size, -size);

	glColor3f(0.0f, 0.0f, 1.0f );
	glNormal3f(1,0,0);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, size, -size);
	glVertex3f(-size, size, size);
	glVertex3f(-size, -size, size);
	//*/
	glColor3f(1.0, 1.0, 1.0 );
	glNormal3f(0,-1,0);
	glTexCoord2f(0 + s, 0 + t);
	glVertex3f(-ssize, size, -ssize);
	glTexCoord2f(1 + s, 0 + t);
	glVertex3f(ssize, size, -ssize);
	glTexCoord2f(1 + s, 1 + t);
	glVertex3f(ssize, size, ssize);
	glTexCoord2f(0 + s, 1 + t);
	glVertex3f(-ssize, size, ssize);
	
	
	
	
	

	
	/*
	glColor3f(1.0, 1.0, 0.0 );
	glNormal3f(0,0,-1);
	glVertex3f(size, size, size);
	glVertex3f(size, -size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(-size, size, size);

	glColor3f(1.0, 0.0, 1.0);
	glNormal3f(0,0,1);
	glVertex3f(-size, -size, -size);
	glVertex3f(size, -size, -size);
	glVertex3f(size, size, -size);
	glVertex3f(-size, size, -size);
	//*/
	glEnd();

	//glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glPopMatrix();


}

void drawbox(int a)
{
	
	float size = BOX_SIZE;
	
	glPushMatrix();
	if(a)
		glTranslatef(test[chaseplane].getX(), test[chaseplane].getY(), test[chaseplane].getZ());

	//glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

	glBegin(GL_QUADS);
	
	glColor3f(0.0f, 1.0f, 1.0f );
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(size, -size, -size);
	glVertex3f(size, -size, size);
	glVertex3f(size, size, size);
	glVertex3f(size, size, -size);

	glColor3f(0.0f, 0.0f, 1.0f );
	glNormal3f(1,0,0);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, size, -size);
	glVertex3f(-size, size, size);
	glVertex3f(-size, -size, size);

	glColor3f(0.0, 1.0, 0.0 );
	glNormal3f(0,-1,0);
	glVertex3f(-size, size, -size);
	glVertex3f(size, size, -size);
	glVertex3f(size, size, size);
	glVertex3f(-size, size, size);

	glColor3f(1.0, 0.0, 0.0 );
	glNormal3f(0,1,0);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, -size, size);
	glVertex3f(size, -size, size);
	glVertex3f(size, -size, -size);

	glColor3f(1.0, 1.0, 0.0 );
	glNormal3f(0,0,-1);
	glVertex3f(size, size, size);
	glVertex3f(size, -size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(-size, size, size);

	glColor3f(1.0, 0.0, 1.0);
	glNormal3f(0,0,1);
	glVertex3f(-size, -size, -size);
	glVertex3f(size, -size, -size);
	glVertex3f(size, size, -size);
	glVertex3f(-size, size, -size);

	glEnd();

	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glPopMatrix();


}

void chasecamabove(int i)
{
	test = Genomes->getPlane(i);
	MyVector heading(0,0,0);
	MyVector perpY(0,0,0);
	float posX, posY, posZ;

	posX = test->getX();
	posY = test->getY();
	posZ = test->getZ();

	heading.setMag(test->getHeadingVec().getMag()); 
	heading.setX(test->getHeadingVec().getX()); 
	heading.setY(test->getHeadingVec().getY()); 
	heading.setZ(test->getHeadingVec().getZ()); 

	perpY.setMag(test->getPerpVecY().getMag()); 
	perpY.setX(test->getPerpVecY().getX()); 
	perpY.setY(test->getPerpVecY().getY()); 
	perpY.setZ(test->getPerpVecY().getZ()); 

	gluLookAt(posX-heading.getX()*2 + perpY.getX()*.5, 
			  posY-heading.getY()*2 + perpY.getY()*.5, 
			  posZ-heading.getZ()*2 + perpY.getZ()*.5,
			  posX + perpY.getX()*.5, posY + perpY.getY()*.5, posZ + perpY.getZ()*.5,
			  perpY.getX(), perpY.getY(), perpY.getZ());


}

void chasecamfloppy(int i)
{
	test = Genomes->getPlane(i);
	MyVector vel(0,0,0);
	MyVector perpY(0,0,0);
	float posX, posY, posZ;

	posX = test->getX();
	posY = test->getY();
	posZ = test->getZ();

	vel.setMag(test->getGlobalTargetVec().getMag()); 
	vel.setX(test->getGlobalTargetVec().getX()); 
	vel.setY(test->getGlobalTargetVec().getY()); 
	vel.setZ(test->getGlobalTargetVec().getZ()); 

	perpY.setMag(test->getPerpVecY().getMag()); 
	perpY.setX(test->getPerpVecY().getX()); 
	perpY.setY(test->getPerpVecY().getY()); 
	perpY.setZ(test->getPerpVecY().getZ()); 

	gluLookAt(posX-vel.getX()*2 + perpY.getX()*.5, 
			  posY-vel.getY()*2 + perpY.getY()*.5, 
			  posZ-vel.getZ()*2 + perpY.getZ()*.5,
			  posX + perpY.getX()*.5, posY + perpY.getY()*.5, posZ + perpY.getZ()*.5,
			  perpY.getX(), perpY.getY(), perpY.getZ());


}

void chasecamfixed(int i)
{
	test = Genomes->getPlane(i);
	float posX, posY, posZ;

	posX = test->getX();
	posY = test->getY();
	posZ = test->getZ();

	
	gluLookAt(posX, posY, posZ - zoom,
			  posX, posY, posZ,
			  0, 1, 0);
	


}

void display(void)
{
	//cout<<"I'm displaying!"<<endl;

	myCount++;
	countCam++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	
	if(myCount>GENERATION_TIME)
	{
		myCount = 0;
		init();
	}

	if(myCount % RESET_TIME == 0)
	{
      Genomes->resetTargets();
	}


	//drawbox();
	int a = 0;
	
	if(keys['l'])
	//if(countCam>10)
	{
		keys['l'] = false;
		camera++;
		countCam=0;
	}
	if(camera>3)
		camera = 0;
	switch(camera)
	{
	case 0:
		glTranslatef(0,0,zoom);
		break;
	case 1:
		chasecamabove(chaseplane);
		a = 1;
		break;
	case 2:
		chasecamfloppy(chaseplane);
		a = 1;
		break;
	case 3:
		chasecamfixed(chaseplane);
		break;
	}
	//*
	glPushMatrix();
	float pos[4] = {0,1,0,0};
	//glTranslatef(5,5,5);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glPopMatrix();
	//*/
	
	
	//drawbox(0);
	drawplain();

	if(keys['r'])
		zoom+=.1;
	if(keys['f'])
		zoom-=.1;
	if(keys['z'])
		zrot+=1;
	if(keys['x'])
		zrot-=1;
	if(keys['c'])
		xrot+=1;
	if(keys['v'])
		xrot-=1;

	if(keys['y'])
	//if(countCam>10)
	{
		keys['y'] = false;
		chaseplane++;
		if(chaseplane>= *dogs)
			chaseplane = 0;

		countCam=0;
		printf("You are viewing Plane[%d]\n", chaseplane );
	}

	if(keys['h'])
	//if(countCam>10)
	{
		keys['h'] = false;
		chaseplane--;
		if(chaseplane<0)
			chaseplane = (*dogs)-1;
		countCam=0;
		printf("You are viewing Plane[%d]\n", chaseplane );
	}

	if(keys['p'])
		init();

	if(keys['w'] || saveCount % saveInc == 0)
	{
      Genomes->writePlanes("population.dat");
	  saveCount = 1;
	}

	if(keys['a'])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(keys['s'])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	//printf("fuckyou before update and draw!\n");

	/*if(keys['m'])
		test[i]->setPrinting(1);
	else
		test[i]->setPrinting(0);*/

	//glPushMatrix();
	
	Genomes->UpdatePopulation();
	//test[i]->update();
	
	//printf("fuckyou after update\n");
	
	Genomes->DrawPopulation(planelist, texture);
	
	///printf("fuckyou after draw pop\n");
	
	Genomes->DrawTargets(planelist);
	
	//printf("fuckyou after draw targets\n");	
	//test[i]->transform();

	//glColor3f(0, 0, 1);
	//glCallList(planelist);

	//glPopMatrix();
	

	/*
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();
	//*/
	//printf("fuckyou100\n");

	glFlush();
	glutSwapBuffers();
}

void init(void)
{
	int i;

	myCount = 0;

	if(generationCount != 0)
	  Genomes->NextGeneration();
	//printf("8) out of NextGeneration() call\n");

	generationCount++;
	cout<<endl<<"generation : "<<generationCount<<endl;
	saveCount++;

	chaseplane = 0;
	
	Genomes->initPlanes();
	Genomes->resetTargets();
	//printf("10) out of initPlanes()");
	

	for(i=0; i<256; i++)
		keys[i] = 0;

	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	GLfloat black[4] = {0,0,0,1};
	GLfloat white[4] = {1,1,1,1};

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	GLfloat lightpos[4] = { 0.0, 1.0, 0.0, 0.0 };
	GLfloat lightamb[4] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightspec[4] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightdif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor3f(1.0, 1.0, 1.0);
	glClearColor(0.0, 0.3, 1.0, 1.0);
	glClearDepth(1.0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_NORMALIZE);
	//glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

}

void reshape(int width, int height)
{
	
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();			
}


void keydown(unsigned char key, int x, int y)
{
	keys[key] = 1;
}
void keyup(unsigned char key, int x, int y)
{
	keys[key] = 0;
}

void idle(void)
{
	glutPostRedisplay();
}

void fakeDisplay() { cout<<"fakeDisplay!"<<endl; glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glutSwapBuffers(); }

int main(int argc, char *argv[])
{
	//sphere = gluNewQuadric();
	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	
	glutCreateWindow("Test");

	glutDisplayFunc(display);
	glutKeyboardFunc(keydown);
	glutKeyboardUpFunc(keyup);
	glutReshapeFunc(reshape);
	//glutIdleFunc(idle);
	glutIdleFunc(display);
	
	//glutInit(argc, argv);

	
	//glutDisplayFunc(display);

		
	Genomes = new GenePool( dogs );
	sphere = gluNewQuadric();
	
	init();

	
	if( !LoadTGA("cloud.tga", 0) ||
		!LoadTGA("land.tga", 1) ||
		!LoadTGA("landtexture.tga", 2) ||
		!LoadTGA("sky.tga", 3) ||
		!LoadTGA("prop.tga", 4) ||
		!LoadTGA("gunflare.tga", 5) )
	{
		cout<<"error loading textures!"<<endl;
	}
	else
	{
		cout<<"textures loaded..."<<endl;
	}
	

	cout<<"input number of updates per generation: ";
	cin>>GENERATION_TIME;
	
	planelist =  Gen3DObjectList();
	glutSwapBuffers();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
