#pragma once

class Texture
{
public:
	Texture(ID3D11Device* pDevice, const std::string& path);
	~Texture();
	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept = delete;

	static Texture* LoadFromFile(const std::string& path);
	dae::ColorRGB Sample(const dae::Vector2& uv) const;

	ID3D11ShaderResourceView* GetResourceView() const; 

private:
	Texture(SDL_Surface* pSurface);

	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pResourceView;

	SDL_Surface* m_pSurface{ nullptr };
	uint32_t* m_pSurfacePixels{ nullptr };
};
