#pragma once

#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include <vector>

#include "SoftTimer.h"


typedef struct
{
	float x;
	float y;
	float th;
} RobotPose;

typedef struct
{
	float xo;
	float yo;
	
	float R_;
	float r_;
} Obstacle;

class softDiffRobot
{
public:
	static constexpr double delta = 0.001;

	softDiffRobot(double dT, int pathLen=20u) :
		x(0.0), y(0.0), th(0.0), dT(dT), v(0.0), om(0.0), odometryTimer(dT, 1, NULL, true), hx(0.0), hy(0.0), L(0.1), ux(0.0), uy(0.0), gradx(0.0), 
		grady(0.0)
	{
		if (pathLen <= 0) pathLen = 1;
		this->pathLen = pathLen;
	}

	void vOdometryUpdate();

	void vFeedbackLin();

	void vAvoidCollision();

	void vOdometryEvent(double v, double omega);
	void vOdometryEventFBlin(double ux, double uy);

	void vSetOdometry(double x, double y, double th);
	void xGetOdometry(double &x, double &y, double &th) const;

	void xGetZpoint(double &hx, double &hy) const;

	void    vSetL(double L);
	double  xGetL();

	void xPrintOdometry() const;

	void xAddPose2Path();

	void dVxy();
	double dV(double l_norm, float R_, float r_);

	SoftTimer odometryTimer;

	std::vector<Obstacle> Obstacles;

	std::vector <RobotPose> RobotTrip;
	std::vector <RobotPose> RobotZpointTrip;
protected:
	double x, y, th;
	
	double v, om;
	double dT;

	double ux, uy;

	double L;
	double hx, hy;

	double gradx, grady;

	int pathLen;
};

