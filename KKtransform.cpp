#include "KKtransform.h"

void KKtransform::start(std::string filename) {
	inOut.readInput("input.txt");
	inOut.readData();
	inOut.prepareOut();
	gsl_set_error_handler_off();
	return;
}

void KKtransform::end() {
	interpolate();
	transform();
	inOut.writeData(txt);
	//if (inOut.check) check();
	gsl_spline_free(spline);
	gsl_interp_accel_free(acc);
	gsl_integration_workspace_free (wspace);
	return;
}

void KKtransform::interpolate() {
	acc = gsl_interp_accel_alloc();
	spline = gsl_spline_alloc (gsl_interp_cspline, inOut.NpointsIn);
	gsl_spline_init(spline, inOut.dataIn[0], inOut.dataIn[1], inOut.NpointsIn);
	return;
}

void KKtransform::transform() {
	double result, error;
	double omegaMax = 0.9*inOut.dataIn[0][inOut.NpointsIn - 1];
	int status = 0;

	omega = 0.0;

	wspace = gsl_integration_workspace_alloc (wlimit);
	gsl_function F;
	F.function = &func;

	for (int i = 1; i < inOut.NpointsOut; ++i) {
		omega = inOut.dataOut[0][i];
		F.params = this;
		status = gsl_integration_qawc (&F, -omegaMax, omegaMax, omega, 0, inOut.eps,
										wlimit, wspace, &result, &error);
		if (status == GSL_EROUND) {
			std::cout << "error: " << status << std::endl;
			inOut.dataOut[1][i] = -1;
		}
		else {
			result = -1.0/M_PI*result;
			inOut.dataOut[1][i] = result;
	//		std::cout << "w = " << omega << ", sigma = " << result << std::endl;
		}
	}

	for (int i = 1; i < inOut.NpointsOut - 1; ++i) {
		if (inOut.dataOut[1][i] < 0) {
			inOut.dataOut[1][i] = 0.5*(log10(inOut.dataOut[1][i - 1]) + 
									   log10(inOut.dataOut[1][i + 1]) );
			inOut.dataOut[1][i] = pow(10, inOut.dataOut[1][i]);
		}
	}

	return;
}

void KKtransform::check() {
	int status = 0;
	double result, error;
	gsl_spline_free(spline);
	gsl_interp_accel_free(acc);
	// spline interpolation for transformed data
	acc = gsl_interp_accel_alloc();
	spline = gsl_spline_alloc (gsl_interp_cspline, inOut.NpointsOut);

	gsl_spline_init(spline, inOut.dataOut[0], inOut.dataOut[1], inOut.NpointsOut);
	// setup
	double omegaMax = 0.9*inOut.dataOut[0][inOut.NpointsOut - 1];
	double dW = inOut.dataOut[0][1];
	int NpointsCheck = ((int) omegaMax/dW) + 1;
	double** dataCheck = new double*[2];
	dataCheck[0] = new double[NpointsCheck];
	dataCheck[1] = new double[NpointsCheck];
	dataCheck[0][0] = 0;
	dataCheck[1][0] = inOut.dataIn[1][0];
	for (int i = 1; i < NpointsCheck; ++i) {
		dataCheck[0][i] = dataCheck[0][i - 1] + dW;
		dataCheck[1][i] = 0;
	}

	// integrate
	gsl_function F;
	F.function = &checkFunc;
  	for (int i = 1; i < NpointsCheck; ++i) {
  		omega = dataCheck[0][i];
  		F.params = this;
  		status = gsl_integration_qawc (&F, -omegaMax, omegaMax, omega, 0, inOut.eps,
  			                           wlimit, wspace, &result, &error);
  		if (status == GSL_EROUND) {
  			std::cout << "error: " << status << std::endl;
  			result = -1;
  		}
  		else {
  			result = -1.0/M_PI*result;
  		}
  		dataCheck[1][i] = result;
  		std::cout << omega << "   " << result << std::endl;
	}

	// remove improper values
	for (int i = 1; i < NpointsCheck - 1; ++i) {
		if (dataCheck[1][i] < 0) {
			dataCheck[1][i] = 0.5*(log10(dataCheck[1][i - 1]) + 
	                               log10(dataCheck[1][i + 1]) );
			dataCheck[1][i] = pow(10, dataCheck[1][i]);
		}
	}

	double sigma = 0;
	for (int i = 1; i < NpointsCheck; ++i) {
		sigma = sigma + (dataCheck[1][i] - inOut.dataIn[1][i])
		              * (dataCheck[1][i] - inOut.dataIn[1][i]);
	}
	sigma = sigma/NpointsCheck;
	sigma = sqrt(sigma);
	std::cout << "check: sigma = " << sigma << std::endl;
	return;
}

double KKtransform::func(double x, void* params) {
	KKtransform* KK = (KKtransform*) params;
	double w = KK->omega;
	double f = 1.0;
	if (x < 0.0) { x = -x; }
	//if (x < KK->inOut.dataIn[0][1]) {
	//	f = f*(KK->inOut.dataIn[1][1]*x/KK->inOut.dataIn[0][1]);
	//}
	//else {
	f = f*pow(10, gsl_spline_eval (KK->spline, x, KK->acc));
	//}
	return f;
}

double KKtransform::checkFunc(double x, void* params) {
	KKtransform* KK = (KKtransform*) params;
	double w = KK->omega;
	if (x < 0.0) x = -x;
	return gsl_spline_eval (KK->spline, x, KK->acc);
}

int main(int argc, char** argv) {
	KKtransform KKtr;
	KKtr.start("input.txt");
	KKtr.end();
	return 0;
}