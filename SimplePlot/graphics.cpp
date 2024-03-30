#include "graphics.hpp"
#include <fstream>
#include <sstream>
#include "stb_truetype.h"
using namespace std;

GLuint splot::internal::glsl_load_program(const string& vsCode, const string& fsCode)
{
	const char* vsData = vsCode.data();
	const char* fsData = fsCode.data();

	GLuint vsId, fsId;
	vsId = glCreateShader(GL_VERTEX_SHADER);
	fsId = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vsId, 1, &vsData, nullptr);
	glShaderSource(fsId, 1, &fsData, nullptr);

	glCompileShader(vsId);
	glCompileShader(fsId);

	GLint vsStatus, fsStatus;
	glGetShaderiv(vsId, GL_COMPILE_STATUS, &vsStatus);
	glGetShaderiv(fsId, GL_COMPILE_STATUS, &fsStatus);

	if (vsStatus != GL_TRUE) {
		char szLog[1024] = {};
		glGetShaderInfoLog(vsId, sizeof(szLog), nullptr, szLog);
		printf("Error compiling vertex shader: %s\n", szLog);
	}

	if (fsStatus != GL_TRUE) {
		char szLog[1024] = {};
		glGetShaderInfoLog(fsId, sizeof(szLog), nullptr, szLog);
		printf("Error compiling fragment shader: %s\n", szLog);
	}

	if (!vsStatus || !fsStatus) {
		throw exception("development error: vertex or fragment shader failed to compile.");
	}

	GLuint programId = glCreateProgram();
	glAttachShader(programId, vsId);
	glAttachShader(programId, fsId);
	glLinkProgram(programId);

	GLint linkError;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkError);

	if (linkError != GL_TRUE) {
		char szLog[1024] = {};
		glGetProgramInfoLog(programId, sizeof(szLog), nullptr, szLog);
		printf("Error linking shader program: %s\n", szLog);
	}

	if (!linkError) {
		throw exception("development error: shader program linking failed.");
	}

	glDeleteShader(vsId);
	glDeleteShader(fsId);

	return programId;
}



shared_ptr<splot::internal::gl_buffer> splot::internal::glsl_load_points_into_vao_buffer(const vector<double>& x, const vector<double>& y,
	pair<double, double> x_range, pair<double, double> y_range)
{
	if (x.size() != y.size() || x.size() == 0) {
		throw exception("vector(x) and vector(y) must have same point count and must be greater than 0");
	}

	auto buffer = splot::internal::gl_buffer::create_buffer();

	// move data into vec2
	struct v2 {
		float x;
		float y;
	};

	vector<v2> points(x.size());

	for (size_t i = 0; i < points.size(); i++) {
		points[i] = { float(x[i]), float(y[i]) };
	}

	buffer->write(points.data(), sizeof(v2)* points.size());
	buffer->bind();
	buffer->enable_attrib_pointer_f32(0, 2, sizeof(v2));
	splot::internal::gl_buffer::unbind();
	return buffer;
}

optional<string> splot::internal::read_all_text(const string& path) {
	// Open the file for reading
	std::ifstream file(path);

	// Check if the file was opened successfully
	if (!file.is_open()) {
		return std::nullopt;
	}

	// Read the contents of the file into a stringstream
	std::stringstream ss;
	ss << file.rdbuf();

	// Close the file
	file.close();

	// Return the contents of the stringstream as an std::string
	return ss.str();
}

void splot::internal::font_tests()
{
}

shared_ptr<splot::internal::fbo_info> splot::internal::fbo_info::gl_create_framebuffer(uint16_t width, uint16_t height)
{
	GLuint fbo, texId;
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &texId);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shared_ptr<splot::internal::fbo_info> result = make_shared<splot::internal::fbo_info>();
	result->fboId = fbo;
	result->textureId = texId;
	result->width = width;
	result->height = height;
	return result;
}

shared_ptr<splot::internal::gl_buffer> splot::internal::gl_buffer::create_buffer()
{
	return make_shared<gl_buffer>();
}
