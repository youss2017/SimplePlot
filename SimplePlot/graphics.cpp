#define _CRT_SECURE_NO_WARNINGS
#include "graphics.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

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



shared_ptr<splot::internal::gl_buffer> splot::internal::glsl_load_points_into_vao_buffer(const vector<double>& x, const vector<double>& y)
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

// Function to load the font file into memory
static unsigned char* load_file(const char* filename, int* size) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = (unsigned char*)malloc(*size);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory for file: %s\n", filename);
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, *size, file);
	fclose(file);

	return buffer;
}

splot::internal::font_map splot::internal::load_font(const std::string& path)
{
	font_map result;

	int size = 0;
	auto ttfBin = load_file(path.c_str(), &size);
	if (!ttfBin) {
		throw runtime_error("Failed to load font file.");
	}

	FT_Library ft;
	FT_Init_FreeType(&ft);

	FT_Face face;
	FT_New_Face(ft, path.c_str(), 0, &face);

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	map<char, vector<uint8_t>> chBuffers;
	
	int maxWidth = 0;
	int maxRows = 0;

	for (char letter = '\n'; letter <= '~'; ++letter) {
		if (letter == '\n' + 1) {
			letter = ' ';
		}

		FT_Load_Char(face, letter, FT_LOAD_RENDER);

		character c{};
		auto buf = (uint8_t*)face->glyph->bitmap.buffer;
		auto width = face->glyph->bitmap.width;
		auto rows = face->glyph->bitmap.rows;

		glGenTextures(1, &c.TextureID);
		glBindTexture(GL_TEXTURE_2D, c.TextureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			buf
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		c.Size[0] = face->glyph->bitmap.width;
		c.Size[1] = face->glyph->bitmap.rows;
		c.Bearing[0] = face->glyph->bitmap_left;
		c.Bearing[1] = face->glyph->bitmap_top;
		c.Advance = face->glyph->advance.x;
		chBuffers.insert({ letter, vector<uint8_t>(buf, buf + width * rows) });
		result[letter] = c;
		maxWidth = max<int>(maxWidth, width);
		maxRows = max<int>(maxRows, rows);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return result;

}

void splot::internal::render_text(GLuint shaderProgram, const font_map& map, const string& text, float x, float y, float scale, int resolution[2], float rgb[3], bool offsetToTrueCenter)
{
	if (text.empty()) return;
	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), resolution[0], resolution[1]);
	glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), rgb[0], rgb[1], rgb[2]);

	gl_buffer buffer;
	buffer.enable_attrib_pointer_f32(0, 4, sizeof(float) * 4);
	const float copyX = x;
	const float copyY = y;

	struct vertex {
		float x;
		float y;
		float u;
		float v;
	};

	bool firstPass = true;
	int textWidth = 0;
	int lastAdvanceX = 0;
	draw_text:

	for (const auto& letter : text) {

		const auto& ch = map.at(letter);
		
		if (letter == '\n') {
			y -= ch.Size[1] * 1.3 * scale;
			x = copyX;
			continue;
		}
		else if (letter == ' ') {
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
			continue;
		}

		const float xpos = x + ch.Bearing[0] * scale;
		const float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

		const float w = ch.Size[0] * scale;
		const float h = ch.Size[1] * scale;

		if (firstPass && offsetToTrueCenter) {
			lastAdvanceX = (ch.Advance >> 6) * scale;
			textWidth += w;
			continue;
		}

		// update VBO for each character
		vertex vertices[6] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update contents
		buffer.write(vertices, sizeof(vertices), false);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	
	if (firstPass && offsetToTrueCenter) {
		firstPass = false;
		//textWidth -= lastAdvanceX;
		x = copyX - (textWidth * 0.5f);
		y = copyY;
		goto draw_text;
	}

	buffer.bind();
}
