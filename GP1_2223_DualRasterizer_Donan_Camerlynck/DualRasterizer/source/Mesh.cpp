#include "pch.h"
#include "Mesh.h"

#include <cassert>

#include "Effect.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	: m_Vertices{ vertices },
	m_Indices{ indices }
{
	m_WorldMatrix = dae::Matrix::CreateTranslation(m_WorldMatrix.GetTranslation() + dae::Vector3{ 0.f, 0.f, 50.f });
	m_VerticesOut.reserve(m_Vertices.size());

	for (size_t i = 0; i < m_VerticesOut.capacity(); i++)
	{
		m_VerticesOut.emplace_back(Vertex_Out{});
	}
}


Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex>vertices, std::vector<uint32_t> indices)
{
	//Create  effect instance
	m_pEffect = new Effect(pDevice, L"Resources/PosCol3D.fx");
	m_pTechnique = m_pEffect->GetTechnique();
	

	//create textures
	Texture* pDiffuse = new Texture{ pDevice, "Resources/vehicle_diffuse.png" };
	m_pEffect->SetDiffuseMap(pDiffuse);
	Texture* pGlossiness = new Texture{ pDevice, "Resources/vehicle_gloss.png" };
	m_pEffect->SetGlossinessMap(pGlossiness);
	Texture* pSpecular = new Texture{ pDevice, "Resources/vehicle_specular.png" };
	m_pEffect->SetSpecularMap(pSpecular);
	Texture* pNormal = new Texture{ pDevice, "Resources/vehicle_normal.png" };
	m_pEffect->SetNormalMap(pNormal);

	delete pDiffuse;
	pDiffuse = nullptr;
	delete pGlossiness;
	pGlossiness = nullptr;
	delete pSpecular;
	pSpecular = nullptr;
	delete pNormal;
	pNormal = nullptr;

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;



	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	const HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);
	if(FAILED(result))
	{
		assert(false);
	}

	//create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	HRESULT result2 = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if(FAILED(result2))
	{
		return;
	}

	//Create Index buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result2 = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if(FAILED(result2))
	{
		return;
	}
	m_WorldMatrix = dae::Matrix::CreateTranslation(0, 0, 50.f);

}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	//1  Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//2  Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);
	//3  Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	//4  Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//5  Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::UpdateMatrices(const dae::Camera& camera)
{
	m_pEffect->SetMatrix(m_WorldMatrix * camera.invViewMatrix * camera.projectionMatrix);
	m_pEffect->SetWorldMatrix(m_WorldMatrix);
	m_pEffect->SetInvViewMatrix(camera.invViewMatrix);
}


Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pInputLayout->Release();
	m_pVertexBuffer->Release();
	delete m_pEffect;
	m_pEffect = nullptr;
	
	
}

void Mesh::ChangeFilter()
{
	m_pEffect->ChangeFilter();
	m_pTechnique = m_pEffect->GetTechnique();
}

const std::vector<Vertex_Out>& Mesh::GetVerticesOut() const
{
	return m_VerticesOut;
}
const std::vector<Vertex>& Mesh::GetVertices() const
{
	return m_Vertices;
}
const std::vector<uint32_t>& Mesh::GetIndices() const
{
	return m_Indices;
}
