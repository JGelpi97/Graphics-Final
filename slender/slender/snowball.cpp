/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include "snowball.h"


snowball::snowball(GLdouble x, GLdouble z)
{
	ball = new myObject(SPHERE);
	ball->ScaleObject(.1, .1, .1);
	ball->setColor(1, 0, 0);
	ball->TranslateObjectTo(x, .05, z);
	dead = false;
}

void snowball::draw()
{
	ball->draw(GL_POLYGON);
}


GLdouble snowball::distFromPlayer(GLdouble x, GLdouble z)
{
	return sqrt(pow(ball->center[0] - x, 2) + pow(ball->center[2] - z, 2));
}

bool snowball::isDead()
{
	return dead;
}

void snowball::killEmElephantMan()
{
	dead = true;
}

snowball::~snowball(void)
{
	delete ball;
}
