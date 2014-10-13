/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include "slender.h"


slender::slender(void)	//Make slenderbro, bro
{
	base = new myObject(SPHERE);
	base->ScaleObject(0.3, 0.3, 0.3);
	base->TranslateObjectTo(0, 0.25, 0);
	base->setColor(.9, .9, 1);

	look[0] = base->u[0];
	look[1] = base->u[1];
	look[2] = base->u[2];

	middle = new myObject(SPHERE);
	middle->ScaleObject(0.2, 0.2, 0.2);
	middle->TranslateObjectTo(0, 0.6, 0);
	middle->setColor(.9, .9, 1);

	
	head = new myObject(SPHERE);
	head->ScaleObject(0.1, 0.1, 0.1);
	head->TranslateObjectTo(0, 0.85, 0);
	head->setColor(.9, .9, 1);


	show_slender = false;

	distFromPlayer = DEFAULT_SLENDER_DISTANCE;

	show_slender_timer = DEFAULT_SHOW_TIMER;
	hide_slender_timer = DEFAULT_HIDE_TIMER;

	slender_move_rate = DEFAULT_SLENDER_MOVE_RATE;

	pos[0] = 0;
	pos[1] = 0.6;
	pos[2] = 0;
	
}

void slender::draw()
{
	base->draw(GL_POLYGON);
	middle->draw(GL_POLYGON);
	head->draw(GL_POLYGON);

}
	
slender::~slender(void)
{
	delete base;
	delete middle;
	delete head;
}

void slender::increaseMoveRate()
{
	slender_move_rate += float(0.02)/float(8);
}

void slender::resetHideSlenderTimer(int num_pages)
{
	hide_slender_timer = DEFAULT_HIDE_TIMER - num_pages*2;
}

float slender::getHideSlenderTimer()
{
	return hide_slender_timer;
}

void slender::resetShowSlenderTimer()
{
	show_slender_timer = DEFAULT_SHOW_TIMER;
}

float slender::getShowSlenderTimer()
{
	return show_slender_timer;
}

void slender::resetShowSlender()
{
	show_slender = false;
}

void slender::showSlender()
{
	show_slender = true;
}

bool slender::getShowSlender()
{
	return show_slender;
}

void slender::setPosition(float* p)
{
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];

	base->TranslateObjectTo(pos[0], base->center[1], pos[2]);
	middle->TranslateObjectTo(pos[0], middle->center[1], pos[2]);
	head->TranslateObjectTo(pos[0], head->center[1], pos[2]);

}

void slender::rotate(float deg)
{
	float tmp_x = base->center[0];
	float tmp_z = base->center[2];

	base->TranslateObjectTo(0, base->center[1], 0);
	middle->TranslateObjectTo(0, middle->center[1], 0);
	head->TranslateObjectTo(0, head->center[1], 0);

	base->RotateObjectAboutCentralAxis(deg);
	middle->RotateObjectAboutCentralAxis(deg);
	head->RotateObjectAboutCentralAxis(deg);

	base->TranslateObjectTo(tmp_x, base->center[1], tmp_z);
	middle->TranslateObjectTo(tmp_x, middle->center[1], tmp_z);
	head->TranslateObjectTo(tmp_x, head->center[1], tmp_z);

	look[0] = base->u[0];
	look[1] = base->u[1];
	look[2] = base->u[2];
}

void slender::countDownHideSlenderTimer(float t)
{
	hide_slender_timer -= t;
}

void slender::countDownShowSlenderTimer(float t)
{
	show_slender_timer -= t;
}

void slender::decreaseDistFromPlayer(float move_amount)
{
	distFromPlayer = distFromPlayer - slender_move_rate + move_amount;
}

void slender::setDistFromPlayer(float d)
{
	distFromPlayer = d;
}