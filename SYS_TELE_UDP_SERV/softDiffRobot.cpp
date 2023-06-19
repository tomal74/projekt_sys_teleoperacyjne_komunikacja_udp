#include "softDiffRobot.h"


void softDiffRobot::vSetOdometry(double x, double y, double th)
{
	this->x  = x;
	this->y  = y;
	this->th = th;
}

void softDiffRobot::vOdometryUpdate()
{
	x  += dT * v * cos(th);
	y  += dT * v * sin(th);
	th += dT * om;
}

void softDiffRobot::vOdometryEvent(double v, double omega)
{
	this->v  = v;
	this->om = omega;

	this->odometryTimer.timerProcess();
}

void softDiffRobot::xGetOdometry(double& x, double& y, double& th) const
{
	x  = this->x;
	y  = this->y;
	th = this->th;
}

void softDiffRobot::xPrintOdometry() const
{
	std::cout << std::fixed << std::setprecision(2) << "x: " << x << std::endl
		<< "y: " << y << std::endl
		<< "th: " << th << std::endl;
}