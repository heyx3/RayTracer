#pragma once

#include "OpenGLIncludes.h"
#include <vector>
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "Vectors.h"
#include "RenderInfo.h"

//Represents a set of shaders for some collection of vertices/indices.
class Material
{
public:

	Material(void) { finalized = false; errorMsg = new char[SH::ERROR_MESSAGE_SIZE]; errorMsg[0] = '\0'; SH::CreateShaderProgram(shaderProg); }
	virtual ~Material(void) { SH::DeleteShaders(&shaderObjects); delete errorMsg; }

	void Enable(void) const { SH::UseShader(shaderProg); }

	const char * const GetShaderErrorMessage(void) const { return errorMsg; }

	void DrawVertices(PrimitiveTypes type, const RenderInfo & renderingInfo, const VertexIndexData & vertexData);

	BufferObjHandle GetShaderProgram(void) const { return shaderProg; }

	int GetHashCode(void) const { return shaderProg; }

protected:

	struct RenderArgs { PrimitiveTypes type; const RenderInfo & renderInfo; const VertexIndexData & viData; RenderArgs(PrimitiveTypes pt, const RenderInfo & ri, const VertexIndexData & vid) : type(pt), renderInfo(ri), viData(vid) { } };
	virtual void InitializeRendering(const RenderArgs & args) = 0;
	virtual void SetUpTextures(const RenderArgs & args) = 0;
	virtual void EnableAttributes(const RenderArgs & args) = 0;
	virtual void DisableAttributes(const RenderArgs & args) = 0;

	enum ShaderTypes
	{
		VertexShader,
		FragmentShader,
	};
	bool AddShader(ShaderTypes shaderType, const char * shaderText);
	bool FinalizeShaders(void);

	UniformLocation GetUniformLocation(const char* uniformName)
	{
		GLint loc;
		if (RenderDataHandler::GetUniformLocation(shaderProg, uniformName, loc))
			return loc;
		
		SetErrorMessage(DH::GetErrorMessage());
		return DH::INVALID_UNIFORM_LOCATION;
	}

	void SetErrorMessage(const char * const msg) { strncpy(errorMsg, msg, SH::ERROR_MESSAGE_SIZE); }

private:
	
	GLenum GetShaderType(ShaderTypes st)
	{
		switch (st)
		{
			case ShaderTypes::VertexShader: return GL_VERTEX_SHADER;
			case ShaderTypes::FragmentShader: return GL_FRAGMENT_SHADER;
		}

		throw 1;
	}

	typedef std::vector<GLuint> ShaderObjList;
	typedef ShaderHandler SH;
	typedef RenderDataHandler DH;

	mutable char * errorMsg;

	BufferObjHandle shaderProg;

	ShaderObjList shaderObjects;

	bool finalized;
};