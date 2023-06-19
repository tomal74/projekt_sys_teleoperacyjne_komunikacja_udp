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

	xAddPose2Path();
}

void softDiffRobot::vFeedbackLin()
{
	hx = x + L * cos(th);
	hy = y + L * sin(th);

	v  = cos(th) * ux + sin(th) * uy;
	om = (1 / L) * (-sin(th) * ux + cos(th) * uy);
}

void softDiffRobot::vAvoidCollision()
{
	dVxy();

	ux -= gradx;	
	uy -= grady; 
}

void softDiffRobot::vOdometryEvent(double v, double omega)
{
	this->v  = v;
	this->om = omega;

	this->odometryTimer.timerProcess();
}

void softDiffRobot::vOdometryEventFBlin(double ux, double uy)
{
	this->ux = ux;
	this->uy = uy;

	this->odometryTimer.timerProcess();
}

void softDiffRobot::xGetOdometry(double& x, double& y, double& th) const
{
	x  = this->x;
	y  = this->y;
	th = this->th;
}

void softDiffRobot::xGetZpoint(double &hx, double &hy) const
{
	hx = this->hx;
	hy = this->hy;
}

void softDiffRobot::vSetL(double L)
{
	this->L = L;
}

double softDiffRobot::xGetL()
{
	return this->L;
}

void softDiffRobot::xPrintOdometry() const
{
	std::cout << std::fixed << std::setprecision(2) << "x: " << x << std::endl
		<< "y: " << y << std::endl
		<< "th: " << th << std::endl;
}

void softDiffRobot::xAddPose2Path()
{
	// dla punktu prowadzenia (miedzy kolami)
	RobotPose TPose = { x, y, th };
	RobotTrip.push_back(TPose);
	if (RobotTrip.size() > pathLen) RobotTrip.erase(RobotTrip.begin());

	// dla punktu Z
	RobotPose TzPose = { hx, hy, th };
	RobotZpointTrip.push_back(TzPose);
	if (RobotZpointTrip.size() > pathLen) RobotZpointTrip.erase(RobotZpointTrip.begin());
}

void softDiffRobot::dVxy()
{	
	gradx = 0.0f;	grady = 0.0f;

	for (auto & itr : Obstacles)
	{
		float lx = hx - itr.xo;
		float ly = hy - itr.yo;

		double l_norm = sqrt(lx * lx + ly * ly);

		double B = dV(l_norm, itr.R_, itr.r_);

		if (l_norm > 0.0f)
		{
			double dirx = lx / l_norm;
			double diry = ly / l_norm;

			gradx += B * dirx;
			grady += B * diry;
		}
		else
		{
			gradx += 0.0;
			grady += 0.0;
		}
	}
}

double softDiffRobot::dV(double l_norm, float R_, float r_)
{
	double out;
	if (l_norm < r_) out = 0;
	else if (l_norm > R_ - delta) out = 0;
	else out = -(R_ - r_) * pow(M_E, ((l_norm - r_) / (l_norm - R_)))   /   pow(( - 1 + pow(M_E, ((l_norm - r_) / (l_norm - R_)))) , 2) 
		/ pow(  (l_norm - R_), 2  );
	return out;
}
//out = -(R - r) * exp((l - r) / (l - R)) / (    -1 + exp((l - r) / (l - R))   ) ^ 2 / (l - R) ^ 2;