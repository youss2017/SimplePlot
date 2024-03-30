#pragma once
#include <vector>
#include <string>

namespace splot {
	using namespace std;

	enum class PlotMode {
		Point,
		Line
	};

	struct figure_s;

	std::vector<double> linspace(double start, double end, size_t num);

	// Function to create a vector of ones
	std::vector<double> ones(size_t n);

	// Function to create a matrix of ones
	std::vector<std::vector<double>> ones(size_t rows, size_t cols);

	// Function to create a vector of zeros
	std::vector<double> zeros(size_t n);

	// Function to create a matrix of zeros
	std::vector<std::vector<double>> zeros(size_t rows, size_t cols);

	double maxv(const std::vector<double>& vec);
	double minv(const std::vector<double>& vec);

	figure_s* create_figure(const std::string& title, size_t width, size_t height);
	void close_figure(figure_s* figure);
	void center_figure();
	void set_current_figure(figure_s* figure);

	void subplot(int rows, int columns, int index);
	void plot(const vector<double>& x, const vector<double>& y, PlotMode mode);
	void polarplot(const vector<double>& angle, const vector<double>& radius, PlotMode mode);
	void xlabel(const std::string& name);
	void ylabel(const std::string& name);
	void xlim(double xmin, double xmax);
	void ylim(double ymin, double ymax);
	void title(const std::string& name);

	void line_color(float r, float g, float b);

	void update(bool presist);

}