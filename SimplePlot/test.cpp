#include <iostream>
#include "splot.hpp"
#include <numbers>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <iomanip>

using namespace std;

constexpr double PI = 3.141592654;

int main() {
	cout << setprecision(5);

	// Compute Current distrbution on linear wire
	double a = 0.1; // radius, meters
	double length = 1000; // 1 meter

	const int segmentCount = 350;
	const double delta = length / (double)segmentCount;

	constexpr double e0 = 8.854187812810e-12;

	vector<vector<double>> matrix;

	for (int i = 0; i < segmentCount; i++) {
		double ym = (i * delta) + (delta / 2.0);

		vector<double> row;
		for (int j = 0; j < segmentCount; j++) {
			double integrationStart = j * delta;
			double integrationEnd = (j + 1) * delta;
			double step = delta / 10000.0;
			double sum = 0.0;
			for (double t = integrationStart; t < integrationEnd; t += step) {
				double yprime = t;
				double ym_minus_yprime = ym - yprime;
				double R = sqrt((ym_minus_yprime * ym_minus_yprime) + a * a);
				double invR = delta / R;
				sum += invR * (1.0/10000.0);
			}
			row.push_back(sum);
		}
		matrix.push_back(row);
	}

	//printMatrix("Z[mn]", matrix);

	auto zmn_inverse = splot::invert_matrix(matrix);

	//printMatrix("Z^-1[mn]", zmn_inverse);

	auto Vmn = splot::zeros(zmn_inverse.size());
	for (int i = 0; i < matrix.size(); i++) {
		Vmn[i] = 1e3 * 4.0 * PI * e0; // kiloVolt source
	}

	//vector<double> data = { 0.8719,
	//0.8019,
	//0.7905,
	//0.8020,
	//0.8732 };

	auto data = splot::matrix_vector_multiplication(zmn_inverse, Vmn);

	vector<double> xaxis = splot::linspace(0, length, 5000);
	vector<double> yaxis = splot::zeros(xaxis.size());
	for (int i = 0; i < xaxis.size(); i++) {
		double position = (i / (double)xaxis.size()) * length;
		int valueIndex = (int)(position / delta);
		yaxis[i] = data[valueIndex];
	}
	splot::export_csv("C:/users/youssef/desktop/mom_zmn.csv", matrix);
	splot::export_csv("C:/users/youssef/desktop/mom_zmn_inverse.csv", zmn_inverse);
	splot::export_csv("C:/users/youssef/desktop/mom_data.csv", xaxis, yaxis);
	splot::plot(xaxis, yaxis, splot::PlotMode::Line);
	splot::xlim(-0.2, 1.2);
	splot::update(true);

}