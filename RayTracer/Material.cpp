#include "Material.h"

bool Material::AddShader(ShaderTypes shaderType, const char * shaderText)
{
	if (finalized)
	{
		SetErrorMessage("Already finalized the shader!");
		return false;
	}

	GLuint shaderObj;
	if (!SH::CreateShader(shaderProg, shaderObj, shaderText, GetShaderType(shaderType)))
	{
		SetErrorMessage(SH::GetErrorMessage());
		return false;
	}

	ShaderObjList::iterator it;

	it = shaderObjects.begin();
	it = shaderObjects.insert(it, shaderObj);

	return true;
}
bool Material::FinalizeShaders(void)
{
	if (finalized)
	{
		SetErrorMessage("This material was already finalized!");
		return false;
	}


	if (SH::FinalizeShaders(shaderProg, true))
	{
		SH::DeleteShaders(&shaderObjects);
		finalized = true;
		return true;
	}


	SetErrorMessage(SH::GetErrorMessage());
	return false;
}

void Material::DrawVertices(PrimitiveTypes pType, const RenderInfo & rInfo, const VertexIndexData & vid)
{
	RenderArgs ra(pType, rInfo, vid);

	InitializeRendering(ra);

	SetUpTextures(ra);

	RenderDataHandler::BindVertexBuffer(vid.GetVerticesHandle());

	if (vid.UsesIndices())
	{
		DH::BindIndexBuffer(vid.GetIndicesHandle());

		EnableAttributes(ra);

		ShaderHandler::DrawIndexedVertices(pType, vid.GetIndicesCount());
	}
	else
	{
		EnableAttributes(ra);

		ShaderHandler::DrawVertices(pType, vid.GetVerticesCount(), vid.GetFirstVertex());
	}

	DisableAttributes(ra);
}