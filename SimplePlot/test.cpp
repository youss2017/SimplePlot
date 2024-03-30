#include <iostream>
#include "splot.hpp"
#include <numbers>

using namespace std;

constexpr double PI = 3.141592654;

double Et(double theta, double phi) {
	double ks = PI;
	double array_factor = 2 * (cos(ks * sin(theta) * sin(phi)) - cos(ks * sin(theta) * cos(phi)));
	double element_factor = sin(theta);
	return array_factor * element_factor;
}

int main() {
	
	auto angle = splot::linspace(-PI, PI, 100);
	auto radius = splot::zeros(angle.size());

	for (size_t i = 0; i < radius.size(); i++) {
		double ang = angle[i];
		//radius[i] = abs(pow(Et(ang, PI / 2.0), 2.0));
		radius[i] = abs(pow(Et(PI/2.0, ang), 2.0));
	}
	
	auto maxv = splot::maxv(radius);

	for (size_t i = 0; i < radius.size(); i++) {
		double v = (radius[i] / maxv);
	}

	splot::polarplot(angle, radius, splot::PlotMode::Line);

	splot::update(true);
}