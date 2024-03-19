#pragma once
#include <string>
#include <glad/glad.h>
#include <vector>
#include <optional>
namespace splot {

	namespace internal {
		using namespace std;

		struct fbo_info {
			GLuint fboId = 0;
			GLuint textureId = 0;

			~fbo_info() {
				if (fboId) {
					glDeleteFramebuffers(1, &fboId);
					glDeleteTextures(1, &textureId);
				}
			}
		};

		GLuint glsl_load_program(const string& vsCode, const string& fsCode);

		GLuint glsl_load_data_into_vao_buffer(const vector<double>& x, const vector<double>& y,
											pair<double, double> x_range, pair<double, double> y_range);

		fbo_info gl_create_framebuffer(uint16_t width, uint16_t height);

		optional<string> read_all_text(const string& path);

	}

}