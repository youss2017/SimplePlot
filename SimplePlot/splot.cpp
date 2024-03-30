#include "splot.hpp"

using namespace splot;

std::vector<double> splot::linspace(double start, double end, size_t num)
{
	std::vector<double> result(num);
	if (num == 1) {
		result[0] = start;
		return result;
	}

	double step = (end - start) / (num - 1);
	for (int i = 0; i < num - 1; ++i) {
		result[i] = start + i * step;
	}
	result[num - 1] = end; // Ensure last element is exactly equal to 'end'
	return result;
}

// Function to create a vector of ones
std::vector<double> splot::ones(size_t n) {
	std::vector<double> result(n, 1.0);
	return result;
}

// Function to create a matrix of ones
std::vector<std::vector<double>> splot::ones(size_t rows, size_t cols) {
	std::vector<std::vector<double>> result(rows, std::vector<double>(cols, 1.0));
	return result;
}

// Function to create a vector of zeros
std::vector<double> splot::zeros(size_t n) {
	std::vector<double> result(n, 0.0);
	return result;
}

// Function to create a matrix of zeros
std::vector<std::vector<double>> splot::zeros(size_t rows, size_t cols) {
	std::vector<std::vector<double>> result(rows, std::vector<double>(cols, 0.0));
	return result;
}

double splot::maxv(const std::vector<double>& vec) {
	if (vec.empty()) {
		return 0.0; // Return default value in case of an empty vector
	}
	return *std::max_element(vec.begin(), vec.end());
}

// Function to find the minimum element in a vector
double splot::minv(const std::vector<double>& vec) {
	if (vec.empty()) {
		return 0.0; // Return default value in case of an empty vector
	}
	return *std::min_element(vec.begin(), vec.end());
}