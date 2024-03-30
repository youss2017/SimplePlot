#include <iostream>
#include "splot.hpp"
#include <numbers>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

using namespace std;

int main() {
	vector<double> x;
	vector<double> y;

	for (size_t i = 0; i < 100; i++) {
		double t = i / 50.0;
		x.push_back(t);
		double f = sin(2.0 * 3.14 * 1.0 * t);
		if (i == 25) {
			f = std::numeric_limits<double>::quiet_NaN();
		}
		y.push_back(f);
	}

	splot::plot(x, y, splot::PlotMode::Line);

	splot::update(true);
}