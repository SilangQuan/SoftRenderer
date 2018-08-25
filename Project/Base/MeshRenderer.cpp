
#include "MeshRenderer.h"
//#include "Render/ShaderProgram.h"
//#include "Render/RenderContext.h"
//#include "Render/Material.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light/LightManager.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material)
{
	mesh = _mesh;
	material = _material;
}
MeshRenderer::~MeshRenderer()
{
	
}

void MeshRenderer::Render(RenderContext* renderContext)
{
	// ShaderProgram* shaderProgram = material->GetShaderProgram();

	// renderContext->modelMatrix = GetGameObject()->GetTransform()->GetLocalToWorldMatrix();
	// //Bind the material
	// material->Bind(renderContext);


	// //shaderProgram->SetUniform("model", GetGameObject()->GetTransform()->GetLocalToWorldMatrix());


	// //Bind the Vertex Array
	// GLuint vao = mesh->GetVAO();
	// glBindVertexArray(vao);

	// //DrawElements
	// if(mesh->GetIndices().size() > 0)
	// {
	// 	glDrawElements(
	// 		GL_TRIANGLES,
	// 		mesh->GetIndices().size(),
	// 		GL_UNSIGNED_INT,
	// 		0);
	// }
	// else
	// {
	// 	int size = mesh->GetPositions().size();
	// 	glDrawArrays(GL_TRIANGLES, 0, size);
	// }

	// //Unbind the Vertex Array
	// glBindVertexArray(0);
}

Mesh*  MeshRenderer::GetMesh()
{
	return mesh;
}

Material*  MeshRenderer::GetMaterial()
{
	return material;
}

void  MeshRenderer::SetMesh(Mesh* _mesh)
{
	mesh = _mesh;
}

void  MeshRenderer::SetMaterial(Material* _material)
{
	material = _material;
}