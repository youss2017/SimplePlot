#include "splot.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

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

std::vector<std::vector<double>> splot::invert_matrix(const std::vector<std::vector<double>>& mat)
{
	auto swapRows = [](vector<vector<double>>&mat, int row1, int row2) {
		int n = mat.size();
		for (int i = 0; i < n; ++i) {
			double temp = mat[row1][i];
			mat[row1][i] = mat[row2][i];
			mat[row2][i] = temp;
		}
	};
	int n = mat.size();
	// Augmenting the matrix with identity matrix
	vector<vector<double>> inverse(n, vector<double>(n * 2, 0));
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			inverse[i][j] = mat[i][j];
		}
		inverse[i][i + n] = 1;
	}

	// Applying Gaussian Elimination
	for (int i = 0; i < n; ++i) {
		// Swapping rows if the pivot is zero
		if (inverse[i][i] == 0) {
			int j = i + 1;
			while (j < n && inverse[j][i] == 0)
				++j;
			if (j == n) {
				cerr << "Matrix is singular!" << endl;
				return mat; // Cannot invert a singular matrix
			}
			swapRows(inverse, i, j);
		}

		// Make the diagonal elements 1
		double divisor = inverse[i][i];
		for (int j = 0; j < n * 2; ++j) {
			inverse[i][j] /= divisor;
		}

		// Making other elements of the column zero
		for (int j = 0; j < n; ++j) {
			if (i != j) {
				double multiplier = inverse[j][i];
				for (int k = 0; k < n * 2; ++k) {
					inverse[j][k] -= multiplier * inverse[i][k];
				}
			}
		}
	}

	// Extracting the inverted matrix from the augmented matrix
	vector<vector<double>> inverted(n, vector<double>(n, 0));
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			inverted[i][j] = inverse[i][j + n];
		}
	}

	return inverted;
}

std::vector<double> splot::matrix_vector_multiplication(const vector<vector<double>>& matrix, const vector<double>& row)
{
	vector<double> result = splot::zeros(row.size());
	for (int r = 0; r < matrix.size(); r++) {
		double dotProduct = 0.0;
		const auto& matrixRow = matrix[r];
		for (int cell = 0; cell < matrixRow.size(); cell++) {
			dotProduct += row[cell] * matrixRow[cell];
		}
		result[r] = dotProduct;
	}
	return result;
}

void splot::print_matrix(const std::vector<std::vector<double>>& matrix, const std::string& name)
{
	if (name.length() > 0)
		printf("%s=\n[", name.c_str());
	else
		printf("[");
	for (const auto& row : matrix) {
		for (double cell : row) {
			printf("%5.5f, ", cell);
		}
		printf("\b\b;");
		cout << endl;
	}
	printf("]\n");
}

void splot::export_csv(const std::string& filePath, const vector<double>& x, const vector<double>& y, const std::string& xaxisName, const std::string& yaxisName, int decimalPlaces)
{
	std::ofstream out(filePath);
	if (!out) {
		std::cerr << "Could not open file '" << filePath << "' for writing." << endl;
		return;
	}
	if (x.size() != y.size()) {
		std::cerr << "x.size() != y.size()" << endl;
		return;
	}
	out << setprecision(decimalPlaces);
	out << xaxisName << "," << yaxisName << ",\n";
	for (size_t i = 0; i < x.size(); i++) {
		out << x[i] << "," << y[i] << ",\n";
	}
	out.close();
}

void splot::export_csv(const std::string& filePath, const vector<vector<double>>& matrix, int decimalPlaces)
{
	std::ofstream out(filePath);
	if (!out) {
		std::cerr << "Could not open file '" << filePath << "' for writing." << endl;
		return;
	}
	out << setprecision(decimalPlaces);
	for (size_t i = 0; i < matrix.size(); i++) {
		const auto& row = matrix[i];
		for (size_t cell = 0; cell < row.size(); cell++) {
			out << row[cell] << ",";
		}
		out << endl;
	}
	out.close();
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