#include "stdinc.h"
#include "utils.h"

std::string ReadFile(const char* filename)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << "Error opening file: " << filename << std::endl;
		return "";
	}
	std::string text;
	std::string temp;
	while (std::getline(file, temp))
		text += temp + "\n";
	return text;
}

GLuint CreateShader(const char* filename, GLenum type)
{
	std::string source = ReadFile(filename);
	const char* srcPtr = source.c_str();

	GLuint res = glCreateShader(type);
	glShaderSource(res, 1, &srcPtr, NULL);
	glCompileShader(res);

	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		GLint log_length = 0;
		glGetShaderiv(res, GL_INFO_LOG_LENGTH, &log_length);
		char* log = (char*)malloc(log_length);
		glGetShaderInfoLog(res, log_length, NULL, log);
		std::cout << "Error compiling shader: " << log << std::endl;
		free(log);
		glDeleteShader(res);
		return 0;
	}
	return res;
}

GLuint LinkShaders(const GLuint* shaders, unsigned numOfShaders)
{
	GLuint program = glCreateProgram();
	for (unsigned i = 0; i<numOfShaders; ++i)
		glAttachShader(program, shaders[i]);
	glLinkProgram(program);
	return program;
}

GLuint CreateProgram(ShaderInfo* shadersInfo, unsigned numOfShaders)
{
	GLuint* shaders = new GLuint[numOfShaders];
	for (unsigned i = 0; i<numOfShaders; ++i)
		shaders[i] = CreateShader(shadersInfo[i].filename.data(), shadersInfo[i].type);
	unsigned ret = LinkShaders(shaders, numOfShaders);
	for (unsigned i = 0; i<numOfShaders; ++i)
		glDeleteShader(shaders[i]);
	delete[] shaders;
	return ret;
}


void CreatePrograms(const std::vector<ShaderInfo> &shadersInfo, std::vector<ProgramInfo> &programsInfo)
{
	unsigned numOfShaders = shadersInfo.size();
	GLuint* shaders = new GLuint[numOfShaders];
	for (unsigned i = 0; i<numOfShaders; ++i)
		shaders[i] = CreateShader(shadersInfo[i].filename.data(), shadersInfo[i].type);

	for (unsigned i = 0; i < programsInfo.size(); ++i)
	{
		*programsInfo[i].program = glCreateProgram();
		for (unsigned j = 0; j < programsInfo[i].shadersId.size(); ++j)
			glAttachShader(*programsInfo[i].program, shaders[programsInfo[i].shadersId[j]]);
		glLinkProgram(*programsInfo[i].program);
	}
	for (unsigned i = 0; i<numOfShaders; ++i)
		glDeleteShader(shaders[i]);
	delete[] shaders;
}