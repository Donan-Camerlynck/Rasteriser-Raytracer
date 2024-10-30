#pragma once
#include "Camera.h"
#include "DataTypes.h"
#include "Effect.h"

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, std::vector<Vertex>vertices, std::vector<uint32_t> indices);
	Mesh(std::vector<Vertex>vertices, std::vector<uint32_t> indices);
	void Render(ID3D11DeviceContext* pDeviceContext);
	void UpdateMatrices(const dae::Camera& camera);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;
	void ChangeFilter();
	

	void Rotate(float angle, float deltaTime);

	dae::Matrix GetWorldMatrix()const { return m_WorldMatrix; }
	const std::vector<Vertex_Out>& GetVerticesOut() const;
	const std::vector<Vertex>& GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;

private:

	std::vector<Vertex>		m_Vertices;
	std::vector<Vertex_Out>	m_VerticesOut;
	std::vector<uint32_t>	m_Indices;

	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	uint32_t m_NumIndices;
	ID3D11Buffer* m_pIndexBuffer;
	Effect* m_pEffect;
	dae::Matrix m_WorldMatrix;


};

