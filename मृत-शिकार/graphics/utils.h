/*
UTILS.H
-----------------------------
Utilities to compile shaders
*/

// Read file contents to a string
std::string ReadFile(const char* filename);
// Compile a shader to a shader object
GLuint CreateShader(const char* filename, GLenum type);
// Link shaders together to a program
GLuint LinkShaders(const GLuint* shaders, unsigned numOfShaders);

// Information about a shader - TYPE OF SHADER and FILENAME
struct ShaderInfo
{
	GLenum type;
	std::string filename;
};
// Use ShaderInfo array to compile and link shaders together to a program
GLuint CreateProgram(ShaderInfo* shadersInfo, unsigned numOfShaders);

// Information about a program to be used with CreatePrograms
struct ProgramInfo
{
	std::vector<unsigned int> shadersId;
	GLuint * program;
};
// Compile shaders together and link to create number of programs
// This function makes it possible for different programs to share one or more common shader(s)
// Use the shadersId member of ProgramInfo structure to set the indices in the ShaderInfo vector
// to be used for the program
void CreatePrograms(const std::vector<ShaderInfo> &shadersInfo, std::vector<ProgramInfo> &programsInfo);
