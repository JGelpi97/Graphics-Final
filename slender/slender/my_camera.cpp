/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include "my_camera.h"


const long double PI = 3.141592653589793238L;

void myCamera::move(float x, float y, float z)
{
	float move_amount = WALK_SPEED;
	if (isSprinting && sprintTime > 0)	//If you can sprint
	{
		move_amount = SPRINT_SPEED;
		setMoveSpeed(move_amount);
		sprintTime -= .25;
	}
	else
	{

	}
	
	pos[0] += move_amount*x;
	pos[1] += move_amount*y;
	pos[2] += move_amount*z;

	if (isSprinting && sprintTime > 0)
	{
		pos[1] += .005 * sin(1.3 * wave);		
	}
	else
	{
		pos[1] += .005 * sin(.9 * wave);
	}
	
	wave += .2;
	if (.2 * sin(2 * wave) > 3.14)
	{
		wave = 0;
		pos[1] = .6;
	}

	generateT();
}

GLfloat** matrixMult4d(GLfloat A[4][4], GLfloat B[4][4])
{	
	GLfloat** temp = new GLfloat*[4];
	for (int i = 0; i < 4; i++)
	{
		temp[i] = new GLfloat[4];
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float t = 0;
			for (int k = 0; k < 4; k++)
			{
				t += A[i][k]*B[k][j];
			}
			temp[i][j] = t;
		}
	}

	return temp;
}

void myCamera::update()
{
	if (sprintTime <= 0)		//If 0 set sprinting to false for mouse clicking things
	{
		isSprinting = false;
	}
	if (!isSprinting)			//If not sprinting, bring sprint timer back up
	{
		if (sprintTime < 100 && sprintTime >= 20)
		{
			sprintTime += .2;
		}
		else if (sprintTime < 20)
		{
			sprintTime += .05;
		}
		else
		{
			sprintTime = 100;
		}
	}
}

myCamera::myCamera(float posx, float posy, float posz, float atx, float aty, float atz, float upx, float upy, float upz, int t)
{
	move_speed = 0;

	wave = 0;
	isSprinting = false;
	sprintTime = 100;
    type = t;

    fovy = 65.0;
    aspect = 1.0;
    zNear = .1;
    zFar = 200.0;
	top = 5;
	bottom = -5;
	left = -5;
	right = 5;

    pos[0] = posx;
    pos[1] = posy;
    pos[2] = posz;
    pos[3] = 1.0;

    look[0] = atx;
    look[1] = aty;
    look[2] = atz;
    look[3] = 1.0;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    GLfloat* r = normalize(look);

    w[0] = -r[0];
    w[1] = -r[1];
    w[2] = -r[2];

	delete[] r;

    GLfloat up_w = dot_product(up, w);

	// mult_w = (up dot w) * w
	GLfloat *mult_w = scalar_mult(up_w, w);

	// up_sub_mult_w = up - (up dot w)*w;
	GLfloat* up_sub_mult_w = vector_subtract(up, mult_w);

	r = normalize(up_sub_mult_w);

    v[0] = r[0];
    v[1] = r[1];
    v[2] = r[2];

	delete [] r;

    r = cross_product(v, w);
    u[0] = r[0];
    u[1] = r[1];
    u[2] = r[2];

    generateT();
    generateR();
    generateS();
	generateM();

	delete [] mult_w;
	delete [] up_sub_mult_w;
	delete [] r;
}

GLfloat myCamera::dot_product(GLfloat* x, GLfloat* y)
{
    GLfloat r = (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
    return r;
}

GLfloat* myCamera::scalar_mult(float x, GLfloat* y)
{
	GLfloat* r = new GLfloat[3];

	r[0] = x*y[0];
	r[1] = x*y[1];
	r[2] = x*y[2];

    return r;
}

GLfloat* myCamera::vector_subtract(GLfloat* x, GLfloat* y)
{
	GLfloat* r = new GLfloat[3];
	r[0] = x[0]-y[0];
	r[1] = x[1]-y[1];
	r[2] = x[2]-y[2];

    return r;
}

GLfloat* myCamera::cross_product(GLfloat* a, GLfloat* b)
{
	GLfloat* r = new GLfloat[3];
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];

    return r;
}

GLfloat* myCamera::normalize(GLfloat* x)
{
    float mag = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
	GLfloat* r = new GLfloat[3];

	r[0] = x[0]/mag;
	r[1] = x[1]/mag;
	r[2] = x[2]/mag;

    return r;
}

void myCamera::generateT()
{
    // load the Ttrans matrix formatted for the stack
    GLfloat tempT[16] = {1,       0,       0,       0,
                         0,       1,       0,       0,
                         0,       0,       1,       0,
                         -pos[0], -pos[1], -pos[2], 1};

    for (int i = 0; i < 16; i++)
    {
        T[i] = tempT[i];   
    }
    
}

void myCamera::generateR()
{
    // load the Rrot matrix formatted for the stack
    GLfloat tempR[16] = {u[0], v[0], w[0], 0,
                           u[1], v[1], w[1], 0,
                           u[2], v[2], w[2], 0,
                              0,    0,    0, 1};

    for (int i = 0; i < 16; i++)
    {
        R[i] = tempR[i];
    }
}

void myCamera::generateS()
{
    // load the Scaling matrix formatted for the stack
    if (type == PERSP)
    {
		float thetaw = aspect*fovy/180*PI;
		float thetah = fovy/180*PI;

		GLfloat tempS[16] = {1/(tan(thetaw/2)*zFar), 0, 0, 0,
							 0, 1/(tan(thetah/2)*zFar), 0, 0,
							 0, 0,                  1/zFar, 0,
							 0, 0,                  0,     1};

		for (int i = 0; i < 16; i++)
		{
			S[i] = tempS[i];
		}
    }
    else if (type == ORTHO)
    {
		GLfloat tempS[16] = {2/(right - left), 0, 0, 0,
							 0, 2/(top - bottom), 0, 0,
							 0, 0, 1/zFar, 0,
							 0, 0, 0,     1};

		for (int i = 0; i < 16; i++)
		{
			S[i] = tempS[i];
		}
    }
}

void myCamera::generateM()
{
    // load the Mpersp matrix formatted for the stack
	float c = -zNear/zFar;
	GLfloat tempM[16] = {1, 0, 0, 0,
						 0, 1, 0, 0,
					     0, 0, -1/(c+1), -1,
	                     0, 0, c/(c+1), 0};

	for (int i = 0; i < 16; i++)
	{
		M[i] = tempM[i];
	}
}

void myCamera::moveAlongU(float t)
{
	pos[0] += t*u[0];
	pos[1] += t*u[1];
	pos[2] += t*u[2];

	generateT();
}

void myCamera::moveAlongV(float t)
{
	pos[0] += t*v[0];
	pos[1] += t*v[1];
	pos[2] += t*v[2];

	generateT();
}

void myCamera::moveAlongW(float t)
{
	pos[0] += t*w[0];
	pos[1] += t*w[1];
	pos[2] += t*w[2];

	generateT();
}

void myCamera::moveAlongX(float t)
{
	pos[0] += t;

	generateT();
}

void myCamera::moveAlongY(float t)
{
	pos[1] += t;

	generateT();
}

void myCamera::moveAlongZ(float t)
{
	pos[2] += t;

	generateT();
}

void myCamera::MoveForwardBack(float t)
{
	GLfloat f[3] = {w[0], 0, w[2]};
	GLfloat* r = normalize(f);

	pos[0] += t*r[0];
	pos[1] += t*r[1];
	pos[2] += t*r[2];

	delete[] r;

	generateT();
}

void myCamera::MoveLeftRight(float t)
{
	GLfloat f[3] = {u[0], 0, u[2]};
	GLfloat* r = normalize(f);

	pos[0] += t*r[0];
	pos[1] += t*r[1];
	pos[2] += t*r[2];

	delete[] r;

	generateT();
}

void myCamera::moveOrigin(float t)
{
    // move closer/further from world origin
	GLfloat* d = normalize(pos);
	pos[0] += t*d[0];
	pos[1] += t*d[1];
	pos[2] += t*d[2];

	delete[] d;

	generateT();
}

void myCamera::RotateU(float deg)
{
    // rotate around local X
	float theta = deg/180*PI;
	GLfloat Rrot[4][4] = {{u[0], u[1], u[2], 0},
					{v[0], v[1], v[2], 0},
	                {w[0], w[1], w[2], 0},
	                {0,    0,    0,    1}};

	GLfloat R[4][4] = {{1,0,0,0},
				 {0, cos(theta), -sin(theta), 0},
				 {0, sin(theta), cos(theta), 0},
				 {0, 0, 0, 1}};

	GLfloat** temp = matrixMult4d(R, Rrot);

    // update axes, look, and up vectors
	u[0] = temp[0][0];
	u[1] = temp[0][1];
	u[2] = temp[0][2];

	v[0] = temp[1][0];
	v[1] = temp[1][1];
	v[2] = temp[1][2];

    up[0] = v[0];
    up[1] = v[1];
    up[2] = v[2];

	w[0] = temp[2][0];
	w[1] = temp[2][1];
	w[2] = temp[2][2];

    look[0] = -w[0];
    look[1] = -w[1];
    look[2] = -w[2];

	for (int i = 0; i < 4; i++)
	{
		delete [] temp[i];
	}
	delete [] temp;

	generateR();
}

void myCamera::RotateV(float deg)
{
    // rotate around local Y

	float theta = deg/180*PI;
	GLfloat Rrot[4][4] = {{u[0], u[1], u[2], 0},
					{v[0], v[1], v[2], 0},
	                {w[0], w[1], w[2], 0},
	                {0,    0,    0,    1}};

	GLfloat R[4][4] = {{cos(theta), 0, sin(theta), 0},
					{0, 1, 0, 0},
					{-sin(theta), 0, cos(theta), 0},
					{0, 0, 0, 1}};

	GLfloat** temp = matrixMult4d(R, Rrot);

    // update axes, look, and up vectors

	u[0] = temp[0][0];
	u[1] = temp[0][1];
	u[2] = temp[0][2];

	v[0] = temp[1][0];
	v[1] = temp[1][1];
	v[2] = temp[1][2];

    up[0] = v[0];
    up[1] = v[1];
    up[2] = v[2];

	w[0] = temp[2][0];
	w[1] = temp[2][1];
	w[2] = temp[2][2];

    look[0] = -w[0];
    look[1] = -w[1];
    look[2] = -w[2];

	for (int i = 0; i < 4; i++)
	{
		delete [] temp[i];
	}
	delete [] temp;

	generateR();
}

void myCamera::RotateW(float deg)
{
    // rotate around local Z
    
	float theta = deg/180*PI;
	GLfloat Rrot[4][4] = {{u[0], u[1], u[2], 0},
					{v[0], v[1], v[2], 0},
	                {w[0], w[1], w[2], 0},
	                {0,    0,    0,    1}};

	GLfloat R[4][4] = {{cos(theta), -sin(theta), 0, 0},
					{sin(theta), cos(theta), 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}};

	GLfloat** temp = matrixMult4d(R, Rrot);

    // update axes, look, and up vectors

	u[0] = temp[0][0];
	u[1] = temp[0][1];
	u[2] = temp[0][2];

	v[0] = temp[1][0];
	v[1] = temp[1][1];
	v[2] = temp[1][2];

    up[0] = v[0];
    up[1] = v[1];
    up[2] = v[2];

	w[0] = temp[2][0];
	w[1] = temp[2][1];
	w[2] = temp[2][2];

    look[0] = -w[0];
    look[1] = -w[1];
    look[2] = -w[2];

	for (int i = 0; i < 4; i++)
	{
		delete [] temp[i];
	}
	delete [] temp;

	generateR();
}

void myCamera::LookLeftRight(float deg)
{
	double theta = PI*double(deg)/double(180);
    float ux = 0;
    float uy = 1;
    float uz = 0;
    float ux_2 = pow(ux, 2);
    float uy_2 = pow(uy, 2);
    float uz_2 = pow(uz, 2);

    // rodriguez's matrix
    GLfloat R[4][4] = {{ux_2 + cos(theta)*(1 - ux_2), (ux*uy*(1-cos(theta))) - uz*sin(theta), (uz*ux*(1-cos(theta))) + uy*sin(theta), 0},
                     {(ux*uy*(1-cos(theta))) + uz*sin(theta), uy_2 + cos(theta)*(1-uy_2), (uy*uz*(1-cos(theta))) - ux*sin(theta), 0},
                     {(uz*ux*(1-cos(theta))) - uy*sin(theta), (uy*uz*(1-cos(theta))) + ux*sin(theta), uz_2 + cos(theta)*(1-uz_2), 0},
                     {0, 0, 0, 1}};

	GLfloat Rrot[4][4] = {{u[0], u[1], u[2], 0},
					{v[0], v[1], v[2], 0},
	                {w[0], w[1], w[2], 0},
	                {0,    0,    0,    1}};

	GLfloat** temp = matrixMult4d(Rrot, R);

    // update axes, look, and up vectors
	u[0] = temp[0][0];
	u[1] = temp[0][1];
	u[2] = temp[0][2];

	v[0] = temp[1][0];
	v[1] = temp[1][1];
	v[2] = temp[1][2];

    up[0] = v[0];
    up[1] = v[1];
    up[2] = v[2];

	w[0] = temp[2][0];
	w[1] = temp[2][1];
	w[2] = temp[2][2];

    look[0] = -w[0];
    look[1] = -w[1];
    look[2] = -w[2];

	for (int i = 0; i < 4; i++)
	{
		delete [] temp[i];
	}
	delete [] temp;

	generateR();

}

void myCamera::adjustFar(float t)
{
    // move the far clipping plane by t
	if (zFar + t > zNear)
	{
		zFar += t;
	}

	generateS();
	generateM();
}

void myCamera::adjustNear(float t)
{
    // move the near clipping plane by t
	if ((zNear + t) < zFar && (zNear + t) > 0)
	{
		zNear += t;
	}
	generateS();
	generateM();
}

void myCamera::switchType()
{
    // switch camera type, reload S
	if (type == PERSP)
	{
		type = ORTHO;
		generateS();
	}
	else
	{
		type = PERSP;
		generateS();
	}
}

void myCamera::zoomIn()
{
    // zoomIn = decreasing fov
    if (type == PERSP)
    {
        if (fovy - 5 > 10)
        {
            fovy -= 5;
            generateS();
        }
    }
    else if (type == ORTHO)
    {
        if((top - 1) > 0 && (bottom + 1) < 0 && (right - 1) > 0 && (left + 1) < 0)
        {
            top -= 1;
            bottom += 1;
            right -= 1;
            left += 1;
            generateS();
        }
    }
}

void myCamera::zoomOut()
{
    // zoomOut = increasing fov
    if (type == PERSP)
    {
        if (fovy + 5 < 140)
        {
            fovy += 5;
            generateS();
        }
    }
    else if (type == ORTHO)
    {
        top += 1;
        bottom -= 1;
        right += 1;
        left -= 1;
        generateS();
    }
}

void myCamera::increaseHeight()
{
    if (type == PERSP)
    {
        if (fovy + 5 < 140)
        {
            fovy += 5;
            generateS();
        }
    }
    else if (type == ORTHO)
    {
        top += 1;
        bottom -= 1;
        generateS();
    }

}

void myCamera::decreaseHeight()
{
    if (type == PERSP)
    {
        if (fovy - 5 > 10)
        {
            fovy -= 5;
            generateS();
        }
    }
    else if (type == ORTHO)
    {
        if((top - 1) > 0 && (bottom + 1) < 0)
        {
            top -= 1;
            bottom += 1;
            generateS();
        }
    }
}

void myCamera::increaseAspect()
{
    if (type == PERSP)
    {
        aspect = aspect * 4.0 / 3.0;
        generateS();
    }
    else if (type == ORTHO)
    {
        right += 1;
        left -= 1;
        generateS();
    }
}

void myCamera::decreaseAspect()
{
    if (type == PERSP)
    {
        aspect = aspect * 3.0 / 4.0;
        generateS();
    }
    else if (type == ORTHO)
    {
        if((right - 1) > 0 && (left + 1) < 0)
        {
            right -= 1;
            left += 1;
            generateS();
        }
    }
}

void myCamera::printInfo()
{
    printf("Camera info:\n");
    printf("\teye point: %f, %f, %f\n", pos[0], pos[1], pos[2]);
    printf("\tlook: %f, %f, %f\n", look[0], look[1], look[2]);
    printf("\tup: %f, %f, %f\n", up[0], up[1], up[2]);
    printf("\theight angle: %f\n", fovy);
    printf("\taspect ratio: %f\n", aspect);
    printf("\tnear: %f\n", zNear);
    printf("\tfar: %f\n", zFar);
    printf("\n");
}

void myCamera::setMoveSpeed(float m)
{
	move_speed = m;
}

float myCamera::getMoveSpeed()
{
	return move_speed;
}