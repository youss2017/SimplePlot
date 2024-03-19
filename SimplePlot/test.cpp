#include <iostream>
#include "splot.hpp"

using namespace std;

int main() {

	splot::create_figure("Figure 1", 800, 600);

	vector<double> x;
	vector<double> y;

	double Ap = 1.0;
	double An = 0.0;
	double freq = 0.25;
	double omega = 2.0 * 3.14 * freq;
	double a0 = (Ap / 2.0) + (An / 2.0);

	vector<double> an;

	for (size_t i = 1; i <= 100; i++) {
		double value = 2.0 * ((Ap * sin(3.14 * i / 2)) / (3.14 * i) - (An * sin(3.14 * i / 2)) / (3.14 * i));
		if (abs(value) > 1e-6)
			an.push_back(value);
	}

	splot::subplot(2, 2, 1);
	for (size_t i = 0; i < 100; i++) {
		double t = i / 50.0;
		x.push_back(t);
		double f = sin(2.0 * 3.14 * 1.0 * t);
		y.push_back(f);
		//for (size_t n = 1; n <= an.size(); n++) {
		//	f += an[n - 1] * cos(2.0 * 3.14 * n * omega * t);
		//}
		//f = f * (cos(2.0f * 3.14 * t) + cos(2.0f * 3.14 * 2.0 * t) + cos(2.0f * 3.14 * 3.0 * t)) * 1/3.0;
		//y.push_back(cos((2.0 * 3.14 * 3 * t) + 0.1*cos(2.0 * 3.14 * 8 * t) + 0.1*cos(2.0 * 3.14 * 22.0 * t)) + 0.55*cos(2.0*3.14*10.0*t + 3.14/3));
	}

	splot::plot(x, y, splot::PlotMode::Line);
	splot::yrange(-2., 2);
	splot::plot(x, y, splot::PlotMode::Line);
	splot::yrange(-2., 2);
	splot::plot(x, y, splot::PlotMode::Line);
	splot::yrange(-2., 2);
	splot::plot(x, y, splot::PlotMode::Line);
	splot::yrange(-2., 2);

	splot::update(true);

}