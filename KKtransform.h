#pragma once
#include <string>
#include <iostream>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_integration.h>
#include "KKreader.h"


class KKtransform {
public:
	void start(std::string filename);
	void end();
private:
	KKInOut inOut;
	gsl_interp_accel *acc;
	gsl_spline    *spline;
	gsl_integration_workspace *wspace;
	static const size_t wlimit = 10000;
	void interpolate();
	void transform();
	void check();
	double omega;
	static double func(double x, void* params);
	static double checkFunc(double x, void* params);
};