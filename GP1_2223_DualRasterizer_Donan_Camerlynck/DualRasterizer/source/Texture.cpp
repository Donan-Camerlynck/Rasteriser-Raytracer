#include "pch.h"
#include "Texture.h"

#include <cassert>

Texture::Texture(ID3D11Device* pDevice, const std::string& path)
{
	m_pSurface = IMG_Load(path.c_str());
	m_pSurfacePixels = (uint32_t*)m_pSurface->pixels;

	/**Texture*/
	D3D11_TEXTURE2D_DESC desc{}; 
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; 
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subResData{};
	subResData.pSysMem = m_pSurface->pixels; //Pointer to init data
	subResData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);  //Distance in bytes 
	subResData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch); //Doc says this is only for 3D textures, but whatever

	HRESULT result{ pDevice->CreateTexture2D(&desc, &subResData, &m_pTexture) };

	if (FAILED(result))
	{
		assert(false && "Couldn't create 2D texture");
	}

	/**Resource view*/
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; //Must be the same as resource type
	SRVDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pResourceView);
	if (FAILED(result))
	{
		assert(false && "Couldn't create Resource view");
	}

	if (m_pSurface)
	{
		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}
}

Texture::~Texture()
{
	m_pResourceView->Release();
	m_pTexture->Release();
	SDL_FreeSurface(m_pSurface);
	m_pSurface = nullptr;
}

ID3D11ShaderResourceView* Texture::GetResourceView() const
{
	return m_pResourceView;
}

dae::ColorRGB Texture::Sample(const dae::Vector2& uv) const
{
	float x = dae::Clamp(uv.x, 0.f, 1.f);
	float y = dae::Clamp(uv.y, 0.f, 1.f);
	int px = x * m_pSurface->w;
	int py = y * m_pSurface->h;

	Uint8 r{}, g{}, b{};

	SDL_GetRGB(m_pSurfacePixels[px + m_pSurface->w * py], m_pSurface->format, &r, &g, &b);

	return { r / 255.f,g / 255.f,b / 255.f };
}

