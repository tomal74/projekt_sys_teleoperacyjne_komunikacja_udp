#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <iomanip>

#include "SoftTimer.h"


class softDiffRobot
{
public:

	softDiffRobot(double dT) :
		x(0.0), y(0.0), th(0.0), dT(dT), v(0.0), om(0.0), odometryTimer(dT, 1, NULL, true)
	{
	}

	void vOdometryUpdate();

	void vOdometryEvent(double v, double omega);

	void vSetOdometry(double x, double y, double th);
	void xGetOdometry(double &x, double &y, double &th) const;

	void xPrintOdometry() const;

	SoftTimer odometryTimer;

protected:
	double x, y, th;
	
	double v, om;
	double dT;
};

