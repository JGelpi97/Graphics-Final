/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#pragma once
#include "my_object.h"
#include <math.h>

#define CUBE     1
#define HOUSE    2
#define SPHERE   3
#define CYLINDER 4
#define CONE     5
#define TORUS    6
#define WALL     7
#define TREE     8

class snowball
{
public:
	snowball(GLdouble x, GLdouble z);
	~snowball(void);
	myObject *ball;	
	GLdouble distFromPlayer(GLdouble x, GLdouble z);
	void draw();
	bool isDead();
	void killEmElephantMan();
private:
	bool dead;
};

