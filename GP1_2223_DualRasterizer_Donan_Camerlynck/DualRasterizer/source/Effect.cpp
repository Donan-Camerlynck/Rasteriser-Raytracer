#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& path)
{
	m_pEffect = LoadEffect(pDevice, path);
	m_pPointTechnique = m_pEffect->GetTechniqueByName("PointFilterTechnique");
	m_pLinearTechnique = m_pEffect->GetTechniqueByName("LinearFilterTechnique");
	m_pAnisotropicTechnique = m_pEffect->GetTechniqueByName("AnisotropicFilterTechnique");
	if(!m_pPointTechnique->IsValid() || !m_pLinearTechnique->IsValid() || !m_pAnisotropicTechnique->IsValid())
	{
		std::wcout << L"Technique not valid\n";
	}

	m_pMatrixWorldViewProj = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if(!m_pMatrixWorldViewProj->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
	}

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if(!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid\n";
	}

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable not valid\n";
	}

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable not valid\n";
	}

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable not valid\n";
	}

	m_pMatrixViewInv = m_pEffect->GetVariableByName("gViewInv")->AsMatrix();
	if(!m_pMatrixViewInv->IsValid())
	{
		std::wcout << L"m_pMatrixViewInv not valid\n";
	}

	m_pMatrixWorld = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatrixWorld->IsValid())
	{
		std::wcout << L"m_pMatrixWorld not valid\n";
	}

}

Effect::~Effect()
{
	m_pNormalMapVariable->Release();//
	m_pGlossinessMapVariable->Release();//
	m_pSpecularMapVariable->Release();//
	m_pDiffuseMapVariable->Release();//
	m_pMatrixViewInv->Release();
	m_pMatrixWorld->Release();
	m_pMatrixWorldViewProj->Release();
	m_pPointTechnique->Release();
	m_pLinearTechnique->Release();
	m_pAnisotropicTechnique->Release();
	m_pEffect->Release();
	
}

void Effect::SetMatrix(const dae::Matrix& worldViewProjMatr)
{
	m_pMatrixWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProjMatr));
}

void Effect::SetWorldMatrix(const dae::Matrix& worldMatrix)
{
	m_pMatrixWorld->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
}

void Effect::SetInvViewMatrix(const dae::Matrix& invViewMatrix)
{
	m_pMatrixViewInv->SetMatrix(reinterpret_cast<const float*>(&invViewMatrix));
}




ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	switch (m_CurrentFilter)
	{
	case FilterMode::Point:
		return m_pPointTechnique;
	case FilterMode::Linear:
		return m_pLinearTechnique;
	case FilterMode::Anisotropic:
		return m_pAnisotropicTechnique;
	}

	return nullptr;
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}


static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect{ nullptr };

		DWORD shaderFlags = 0;
#if defined(DEBUG) || defined( _DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

		result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
		if(FAILED(result))
		{
			if(pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}
		return pEffect;
	}

void Effect::ChangeFilter()
{
	switch (m_CurrentFilter)
	{
	case FilterMode::Point:
		std::wcout << "Sampler Filter set to Linear\n";
		m_CurrentFilter = FilterMode::Linear;
		break;
	case FilterMode::Linear:
		std::wcout << "Sampler Filter set to Anisotropic\n";
		m_CurrentFilter = FilterMode::Anisotropic;
		break;
	case FilterMode::Anisotropic:
		std::wcout << "Sampler Filter set to Point\n";
		m_CurrentFilter = FilterMode::Point;
		break;
	}
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetResourceView());
	}
}
void Effect::SetGlossinessMap(Texture* pGlossinessTexture)
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetResourceView());
	}
}

void Effect::SetNormalMap(Texture* pNormalTexture)
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalTexture->GetResourceView());
	}
}

void Effect::SetSpecularMap(Texture* pSpecularTexture)
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularTexture->GetResourceView());
	}
}

