
std::string ReadFile(const char* filename);
GLuint CreateShader(const char* filename, GLenum type);
GLuint LinkShaders(const GLuint* shaders, unsigned numOfShaders);

struct ShaderInfo
{
	GLenum type;
	std::string filename;
};
GLuint CreateProgram(ShaderInfo* shadersInfo, unsigned numOfShaders);

struct ProgramInfo
{
	std::vector<unsigned int> shadersId;
	GLuint * program;
};
void CreatePrograms(const std::vector<ShaderInfo> &shadersInfo, std::vector<ProgramInfo> &programsInfo);
