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

	figure_s* create_figure(const std::string& title, size_t width, size_t height);
	void close_figure(figure_s* figure);
	void center_figure();
	void set_current_figure(figure_s* figure);

	void subplot(int rows, int columns, int index);
	void plot(const vector<double>& x, const vector<double>& y, PlotMode mode);
	void xlabel(const std::string& name);
	void ylabel(const std::string& name);
	void xrange(double xmin, double xmax);
	void yrange(double ymin, double ymax);
	void title(const std::string& name);

	void line_color(float r, float g, float b);

	void update(bool presist);

}