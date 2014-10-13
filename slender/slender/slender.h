/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include "my_object.h"

#define CUBE     1
#define HOUSE    2
#define SPHERE   3
#define CYLINDER 4
#define CONE     5
#define TORUS    6
#define WALL     7
#define TREE     8

#define DEFAULT_HIDE_TIMER 20000			//How long he is hidden - 20 sec
#define DEFAULT_SHOW_TIMER 12000			//How long he is shown - 12 sec

#define DEFAULT_SLENDER_MOVE_RATE 0.045

#define DEFAULT_SLENDER_DISTANCE 20

class slender
{
public:
	float distFromPlayer;

	void draw();
	slender(void);
	void resetShowSlenderTimer();
	void countDownShowSlenderTimer(float);
	float getShowSlenderTimer();


	void resetHideSlenderTimer(int);
	void countDownHideSlenderTimer(float);
	float getHideSlenderTimer();

	void showSlender();
	bool getShowSlender();
	void resetShowSlender();

	void increaseMoveRate();
	float getMoveRate();

	void setPosition(float[3]);
	void setLook();

	void decreaseDistFromPlayer(float);
	void setDistFromPlayer(float);
	void getDistFromPlayer();

	void rotate(float);

	float pos[3];
	float look[3];

	myObject *base;
	myObject *middle;
	myObject *head;

	~slender(void);

private:

	float hide_slender_timer;
	float show_slender_timer;

	float slender_move_rate;

	bool show_slender;

};

