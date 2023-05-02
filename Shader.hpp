
#include <GL/glew.h>

#ifndef SHADE
#define SHADE

class Shader{

	private:
		GLuint programID;

	public:
		Shader(){programID = -1;};
		Shader(const char * name);
		GLuint getUniform(const char * uniformName);
		void setUniform(GLuint id, void *data, short type);
		void activate();

};
#endif
