#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include "graphics.hpp"
#include <vector>
#pragma comment(lib, "opengl32.lib")

namespace splot {
	struct figure_s {
		HWND hWnd = nullptr;
		HDC hDc = nullptr;
		HGLRC hGLRC = nullptr;

		GLuint PlotProgramId = 0;
		GLuint FigureProgramId = 0;
		GLuint TextProgramId = 0;
		splot::internal::font_map fontMap;

		int subplot_rows = 1;
		int subplot_column = 1;
		int subplot_index = 0;

		struct curve_data {
			std::string curveTitle;
			std::string xLabel;
			std::string yLabel;
			std::vector<double> x_values;
			std::vector<double> y_values;
			size_t pointCount = 0;
			int subplot_index = 0;
			float rgb[3] = { 0.12f, 0.2f, 0.9f };

			pair<double, double> x_range;
			pair<double, double> y_range;

			shared_ptr<internal::gl_buffer> verticesData;
			shared_ptr<internal::fbo_info> renderTarget;
			
			PlotMode plotMode = PlotMode::Line;
		};
		// Contains data for plotting curves (through quads) on the window framebuffer
		shared_ptr<internal::gl_buffer> figureCurve; 
		vector<curve_data> curves;
	};
}
