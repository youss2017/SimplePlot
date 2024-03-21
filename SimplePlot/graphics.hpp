#pragma once
#include <string>
#include <glad/glad.h>
#include <vector>
#include <optional>
#include <memory>
namespace splot {

	namespace internal {
		using namespace std;

		class gl_buffer {
		public:
			static shared_ptr<gl_buffer> create_buffer();

			gl_buffer() {
				glGenVertexArrays(1, &vaoId);
				glGenBuffers(1, &vboId);
			}

			gl_buffer(gl_buffer&& move) noexcept : vaoId(exchange(move.vaoId, 0)), vboId(exchange(move.vboId, 0))
			{}

			gl_buffer(gl_buffer& copy) = delete;
			gl_buffer& operator=(gl_buffer& copy) = delete;

			gl_buffer& operator=(gl_buffer&& move) noexcept {
				this->~gl_buffer();
				vaoId = exchange(move.vaoId, 0);
				vboId = exchange(move.vboId, 0);
			}

			~gl_buffer() {
				glDeleteVertexArrays(1, &vaoId);
				glDeleteBuffers(1, &vboId);
			}

			void write(const void* pData, size_t length) {
				glBindVertexArray(vaoId);
				glBindBuffer(GL_ARRAY_BUFFER, vboId);
				glBufferData(GL_ARRAY_BUFFER, length, pData, GL_DYNAMIC_DRAW);
				glBindVertexArray(0);
			}

			void bind() {
				glBindVertexArray(vaoId);
			}

			static inline void unbind() {
				glBindVertexArray(0);
			}

			void enable_attrib_pointer_f32(int attribPointer, int vecLength, int stride) {
				bind();
				glBindBuffer(GL_ARRAY_BUFFER, vboId);
				glEnableVertexAttribArray(attribPointer);
				glVertexAttribPointer(attribPointer, vecLength, GL_FLOAT, GL_FALSE, stride, (void*)(size_t)offset);
				offset += vecLength * sizeof(float);
				glBindVertexArray(0);
			}

		public:
			GLuint vaoId;
			GLuint vboId;
		private:
			GLuint offset = 0;
		};

		class fbo_info {
		public:
			GLuint fboId = 0;
			GLuint textureId = 0;
			uint16_t width = 0;
			uint16_t height = 0;

			static std::shared_ptr<fbo_info> gl_create_framebuffer(uint16_t width, uint16_t height);

			~fbo_info() {
				if (fboId) {
					glDeleteFramebuffers(1, &fboId);
					glDeleteTextures(1, &textureId);
				}
			}

			void bind() {
				glBindFramebuffer(GL_FRAMEBUFFER, fboId);
				glViewport(0, 0, width, height);
			}

			static inline void unbind() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			fbo_info() = default;
		};

		GLuint glsl_load_program(const string& vsCode, const string& fsCode);

		shared_ptr<gl_buffer> glsl_load_points_into_vao_buffer(const vector<double>& x, const vector<double>& y,
											pair<double, double> x_range, pair<double, double> y_range);

		optional<string> read_all_text(const string& path);

	}

}