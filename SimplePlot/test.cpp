#include <iostream>
#include "splot.hpp"
#include <numbers>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <iomanip>
#include <complex>
#pragma comment(lib, "freetype.lib")

using namespace std;

constexpr double PI = 3.141592654;

struct point {
	double x;
	double y;
};

int main() {
	cout << setprecision(5);

	std::vector<point> points = {
				{-25,	-0.053},
				{-24,	-0.018},
				{-23,	-0.013 },
				{-22,	-0.011},
				{-21,	-0.011},
				{-20,	-0.009},
				{-19,	-0.008},
				{-18,	-0.007},
				{-17,	-0.007},
				{-16,	-0.006},
				{-15,	-0.006},
				{-14,	-0.004},
				{-13,	-0.004},
				{-12,	-0.004},
				{-11,	-0.003},
				{-10,	-0.003},
				{-9,	-0.003},
				{-8,	-0.002},
				{-7,	-0.002},
				{-6,	-0.001},
				{-5,	-0.001},
				{-4,	-0.001},
				{-3,	-0.001},
				{-2,	0	  },
				{-1,	0	  },
				{0,	0},
				{1,	0.004},
				{2,	0.012},
				{3,	0.019},
				{4,	0.035},
				{5,	0.078},
	};

	vector<double> x;
	vector<double> y;
	for (auto& p : points) {
		x.push_back(p.x);
		y.push_back(p.y);
	}

	splot::create_figure("Schottky Diode I-V Curve", 1280, 720);

	// x(t) = 47cos(2*pi*200t); Fs = 2 kHz
	// 1 cycle of x(t) or Fs/fc
	vector<double> xt;
	vector<double> yt;
	const double Fs = 2000;
	const double fc = 440;
	const int zeroCount = 200;

	for (int n = 0; n < Fs / fc; n++) {
		xt.push_back(n);
		yt.push_back(47.0 * cos(2.0 * 3.14 * fc * n / Fs));
	}

	for (int i = 0; i < zeroCount; i++) {
		xt.push_back(*(--xt.end()) + 1);
		yt.push_back(0);
	}

	const double N = xt.size();
	vector<complex<double>> dft((int)N);
	vector<double> dftMag((int)N);

	for (int k = 0; k < N; k++) {
		for (int n = 0; n < N; n++) {
			dft[k] += yt[n] * exp(complex<double>(0, -1) * double(2.0 * 3.14 * n * k / N));
		}
	}

	for (int i = 0; i < N; i++) {
		dftMag[i] = sqrt(dft[i].real() * dft[i].real() + dft[i].imag() * dft[i].imag()) * (2.0 / N);
	}

	splot::subplot(2, 1, 1);
	splot::plot(xt, yt, splot::PlotMode::Line);
	splot::ylabel("Amplitude");
	splot::xlabel("Sample");
	splot::title("x(t) = 47cos(2pi200t)");
	splot::color(1.0, 0.0, 0.0);
	splot::subplot(2, 1, 2);
	splot::plot(xt, dftMag, splot::PlotMode::Line);
	splot::ylabel("Amplitude");
	splot::xlabel("Sample");
	splot::title("DFT{x(t)}");
	splot::color(0.55, 0.22, 0.35);
	auto maggg = splot::maxv(dftMag);
	splot::ylim(-0.5, maggg + 0.8);

	splot::update(true);

}