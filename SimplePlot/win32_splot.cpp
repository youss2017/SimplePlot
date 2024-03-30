#include "splot.hpp"
#include "win32_def.hpp"
#include <Objbase.h>
#include <algorithm>

using namespace splot;

static figure_s* g_CurrentFigure = nullptr;

static vector<figure_s*> AllFigures;

void win32_render();
void compute_curve_placement(figure_s* figure);

figure_s* splot::create_figure(const std::string& title, size_t width, size_t height)
{
	if (!g_CurrentFigure) {
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		// Register class
		WNDCLASSEXA wc = {};
		wc.cbSize = sizeof(WNDCLASSEXA);
		wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = DefWindowProcA;
		wc.hInstance = GetModuleHandleA(nullptr);
		wc.lpszClassName = "SimplePlotClass";
		RegisterClassExA(&wc); // (TODO): Add error checking
	}
	figure_s* fig = new figure_s();

	fig->hWnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW,
		"SimplePlotClass",
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(int)width, (int)height,
		nullptr,
		nullptr,
		GetModuleHandleA(nullptr),
		nullptr);

	ShowWindow(fig->hWnd, SW_SHOWDEFAULT);
	//fig->hWnd = GetConsoleWindow();

	HDC dc = GetDC(fig->hWnd);

	// Create OpenGL Context
	PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1 };
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	auto format_index = ::ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, format_index, &pfd);
	auto active_format_index = ::GetPixelFormat(dc);
	DescribePixelFormat(dc, active_format_index, sizeof pfd, &pfd);

	auto wgl1 = wglCreateContext(dc);
	wglMakeCurrent(dc, wgl1);

	// Function pointer for wglCreateContextAttribsARB
	typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
	// Load wglCreateContextAttribsARB function

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	int attribs[] = {
		0x2091, 3, // Set OpenGL major version to 3
		0x2092, 3, // Set OpenGL minor version to 3
		0x9126, 0x00000001, // Use the core profile
		0 // Zero-terminated list
	};
	HGLRC wgl = wglCreateContextAttribsARB(dc, NULL, attribs);

	fig->hGLRC = wgl;
	fig->hDc = dc;
	wglMakeCurrent(dc, wgl);
	wglDeleteContext(wgl1);

	auto status = gladLoadGL();

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

	// TODO: Make the shaders as constant string in c++ header file to avoid file loading issues
	fig->PlotProgramId = splot::internal::glsl_load_program(splot::internal::read_all_text("plotvs_shader.glsl").value(), splot::internal::read_all_text("plotfs_shader.glsl").value());
	fig->FigureProgramId = splot::internal::glsl_load_program(splot::internal::read_all_text("figvs_shader.glsl").value(), splot::internal::read_all_text("figfs_shader.glsl").value());

	//GLuint figVaoId, figVboId;
	//glGenVertexArrays(1, &figVaoId);
	//glGenBuffers(1, &figVboId);
	//glBindVertexArray(figVaoId);
	//glBindBuffer(GL_ARRAY_BUFFER, figVboId);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	//glBindVertexArray(0);
	//
	//fig->figureVaoId = figVaoId;

	glEnable(GL_MULTISAMPLE);

	g_CurrentFigure = fig;

	AllFigures.push_back(fig);

	return fig;
}

void splot::close_figure(figure_s* figure)
{
	DestroyWindow(figure->hWnd);
	delete figure;
}

void splot::center_figure()
{
}

void splot::set_current_figure(figure_s* figure)
{
	g_CurrentFigure = figure;
	wglMakeCurrent(g_CurrentFigure->hDc, g_CurrentFigure->hGLRC);
}

void splot::subplot(int rows, int columns, int index)
{
	// (TODO): Add error checking
	g_CurrentFigure->subplot_rows = max(rows, 1);
	g_CurrentFigure->subplot_column = max(columns, 1);
	g_CurrentFigure->subplot_index = index;
}

void splot::plot(const vector<double>& x, const vector<double>& y, PlotMode mode)
{
	// Do we have figure already? yes then plot to the current figure
	// otherwise create new figure and plot to that figure
	if (x.size() == 0 || y.size() == 0 || x.size() != y.size())
		return;
	if (g_CurrentFigure) {
		figure_s::curve_data curve;
		curve.subplot_index = g_CurrentFigure->subplot_index;
		//curve.x_values = x;
		//curve.y_values = y;
		double x_min = *min_element(x.begin(), x.end());
		double x_max = *max_element(x.begin(), x.end());
		double y_min = *min_element(y.begin(), y.end());
		double y_max = *max_element(y.begin(), y.end());
		curve.x_range = { x_min, x_max };
		curve.y_range = { y_min, y_max };
		curve.pointCount = x.size();
		curve.verticesData = splot::internal::glsl_load_points_into_vao_buffer(x, y, curve.x_range, curve.y_range);
		curve.renderTarget = splot::internal::fbo_info::gl_create_framebuffer(800, 600);
		g_CurrentFigure->curves.push_back(curve);
	}
	else {
		create_figure("Figure 1", 640, 480);
		plot(x, y, mode);
	}
}

void splot::polarplot(const vector<double>& angle, const vector<double>& radius, PlotMode mode)
{
	// Do we have figure already? yes then plot to the current figure
	// otherwise create new figure and plot to that figure
	if (angle.size() == 0 || radius.size() == 0 || angle.size() != radius.size())
		return;
	if (g_CurrentFigure) {
		vector<double> x(angle.size());
		vector<double> y(angle.size());
		double max_radius = 0.0;
		for (size_t i = 0; i < x.size(); i++) {
			x[i] = radius[i] * cos(angle[i]);
			y[i] = radius[i] * sin(angle[i]);
			max_radius = max(abs(radius[i]), max_radius);
		}
		max_radius *= 1.02;
		plot(x, y, mode);
		xlim(-max_radius, max_radius);
		ylim(-max_radius, max_radius);
	}
	else {
		create_figure("Figure 1", 640, 480);
		polarplot(angle, radius, mode);
	}
}

void splot::xlabel(const std::string& name)
{
}

void splot::ylabel(const std::string& name)
{
}

void splot::xlim(double xmin, double xmax)
{
	if (!g_CurrentFigure) return;
	auto& curve = g_CurrentFigure->curves;
	if (curve.size() > 0) {
		curve[curve.size() - 1].x_range = { xmin, xmax };
	}
}

void splot::ylim(double ymin, double ymax)
{
	if (!g_CurrentFigure) return;
	auto& curve = g_CurrentFigure->curves;
	if (curve.size() > 0) {
		curve[curve.size() - 1].y_range = { ymin, ymax };
	}
}

void splot::title(const std::string& name)
{
}

void splot::line_color(float r, float g, float b)
{
}

void splot::update(bool presist)
{
	MSG msg;
	while (GetMessageA(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		win32_render();
	}
}

void win32_render()
{
	for (auto& figure : AllFigures) {

		RECT rect;
		GetClientRect(figure->hWnd, &rect);
		wglMakeCurrent(figure->hDc, figure->hGLRC);

		glUseProgram(figure->PlotProgramId);
		GLint lineColorId = glGetUniformLocation(figure->PlotProgramId, "LineColor");
		GLint x_rangeId = glGetUniformLocation(figure->PlotProgramId, "x_range");
		GLint y_rangeId = glGetUniformLocation(figure->PlotProgramId, "y_range");

		GLint curveTextureId = glGetUniformLocation(figure->FigureProgramId, "CurveTexture");

		srand(0xea);
		for (auto& curve : figure->curves) {
			//const auto& xv = curve.x_values;
			//const auto& yv = curve.y_values;
			const auto x_range = curve.x_range;
			const auto y_range = curve.y_range;

			curve.renderTarget->bind();
			curve.verticesData->bind();
			glClearColor(.89f, .89f, .89f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glLineWidth(5.0f);
			glPointSize(5.0f);
			glUniform3f(lineColorId, 0.12f, 0.2f, 0.9f);
			glUniform2f(x_rangeId, (float)curve.x_range.first, (float)curve.x_range.second);
			glUniform2f(y_rangeId, (float)curve.y_range.first, (float)curve.y_range.second);
			glDrawArrays(GL_POINTS, 0, (GLsizei)curve.pointCount);
		}
		internal::gl_buffer::unbind();
		internal::fbo_info::unbind();

		compute_curve_placement(figure);
		figure->figureCurve->bind();
		glUseProgram(figure->FigureProgramId);

		glViewport(0, 0, rect.right, rect.bottom);
		glClearColor(0.57f, 0.37f, 0.33f, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		size_t curveI = 0;
		for (auto& curve : figure->curves) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, curve.renderTarget->textureId);
			glUniform1i(curveTextureId, 0);
			glDrawArrays(GL_TRIANGLES, curveI * 6, 6);
			curveI++;
		}

		glBindVertexArray(0);

		SwapBuffers(figure->hDc);
	}
}

static float map_range(float value, float from_min, float from_max, float to_min, float to_max) {
	// Map value from the range [from_min, from_max] to the range [to_min, to_max]
	return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

void compute_curve_placement(figure_s* figure)
{
	const float padding = 0.1f;
	const float edgePadding = 0.05f;
	float columnCount = (float)figure->subplot_column;
	float rowCount = (float)figure->subplot_rows;
	float curveWidth = (1.0f - (padding + edgePadding * (figure->subplot_column - 1))) / columnCount;
	float curveHeight = (1.0f - (padding + edgePadding * (figure->subplot_rows - 1))) / rowCount;

	struct v2 {
		float x, y;
		float u, v;
	};

	v2 quad[] = {
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 0.0f}
	};

	if (!figure->figureCurve) {
		figure->figureCurve = internal::gl_buffer::create_buffer();
		figure->figureCurve->enable_attrib_pointer_f32(0, 2, sizeof(v2));
		figure->figureCurve->enable_attrib_pointer_f32(1, 2, sizeof(v2));
	}

	vector<v2> vertices;

	for (int c = 0; c < figure->subplot_column; c++) {
		for (int r = figure->subplot_rows - 1; r >= 0; r--) {
			if (c * r >= figure->curves.size()) {
				continue;
			}
			const float columnGap = padding / columnCount;
			const float rowGap = padding / rowCount;
			v2 corner_position = {
				(edgePadding)+(columnGap * c) + (curveWidth * c),
				(edgePadding)+(rowGap * r) + (curveHeight * r),
			};

			v2 origin_position = {
				corner_position.x ,
				corner_position.y ,
			};

			v2 transformed_quad[6] = {};
			memcpy(transformed_quad, quad, sizeof(quad));

			for (size_t i = 0; i < 6; i++) {
				transformed_quad[i].x *= curveWidth;
				transformed_quad[i].y *= curveHeight;
				transformed_quad[i].x += origin_position.x;
				transformed_quad[i].y += origin_position.y;
				vertices.push_back(transformed_quad[i]);
			}
		}
	}

	figure->figureCurve->write(vertices.data(), vertices.size() * sizeof(v2));

}