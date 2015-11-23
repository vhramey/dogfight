#ifndef QUATERNION_H
#define QUATERNION_H

#define EPSILON 1e-12
#define PI		   3.1415926535897932384626433832795f
#define PId2          1.5707963279489
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180 57.2957795130823208767981548141052f

typedef class QUATERNION
	{
	public:
		union
			{
			float q[4];
			struct
				{
				float w, x, y, z;
				};
			};

		//Constructor
		QUATERNION(void);
		QUATERNION(const QUATERNION &);
		QUATERNION(float, float, float);
		QUATERNION(float, float, float, float);
		
		//Misc
		QUATERNION &Reset(void);
		QUATERNION &Set(const QUATERNION &);
		QUATERNION &Set(float, float, float);
		QUATERNION &Set(float, float, float, float);
		QUATERNION &PostMult(const QUATERNION &);
		QUATERNION &MultAndSet(const QUATERNION &, const QUATERNION &);
		QUATERNION &Normalize(void);
		QUATERNION Inversed(void);
		float DotProduct(void);
		float DotProduct(QUATERNION);
		float GetLength(void);
		void GetAxisAngle(float &, float &, float &, float &);
		void GetAxisAngle(double &, double &, double &, double &);
		void GetDirectionVector(float &xx, float &yy, float &zz);
		void Mult(QUATERNION, QUATERNION);
		void GetMatrix(float *mT);
		
	} QUATERNION;

#endif

