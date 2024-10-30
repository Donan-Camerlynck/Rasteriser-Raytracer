#pragma once
#include "Camera.h"
#include "Texture.h"

static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
class Effect final
{
	
public:
	Effect(ID3D11Device* device, const std::wstring& path);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	ID3DX11EffectTechnique* GetTechnique() const;
	ID3DX11Effect* GetEffect() const;
	void ChangeFilter();

	void SetMatrix(const dae::Matrix& worldViewProjMatr);
	void SetWorldMatrix(const dae::Matrix& worldMatrix);
	void SetInvViewMatrix(const dae::Matrix& invViewMatrix);

	void SetDiffuseMap(Texture* pDiffuseTexture);
	void SetGlossinessMap(Texture* pGlossinessTexture);
	void SetSpecularMap(Texture* pGlossinessTexture);
	void SetNormalMap(Texture* pGlossinessTexture);

private:
	
	ID3DX11Effect* m_pEffect;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11EffectMatrixVariable* m_pMatrixWorldViewProj;

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;

	ID3DX11EffectMatrixVariable* m_pMatrixViewInv;
	ID3DX11EffectMatrixVariable* m_pMatrixWorld;

	ID3DX11EffectTechnique* m_pPointTechnique;
	ID3DX11EffectTechnique* m_pLinearTechnique;
	ID3DX11EffectTechnique* m_pAnisotropicTechnique;

	enum class FilterMode
	{
		Point,
		Linear,
		Anisotropic
	};

	FilterMode m_CurrentFilter{ FilterMode::Point };
};