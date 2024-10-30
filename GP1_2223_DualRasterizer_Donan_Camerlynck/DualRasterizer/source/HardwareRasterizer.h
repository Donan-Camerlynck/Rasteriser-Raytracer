#pragma once
#include "DataTypes.h"
#include "Effect.h"
#include "Camera.h"
#include "Mesh.h"

struct SDL_Window;


class HardwareRasterizer final
{
public:
	HardwareRasterizer(SDL_Window* pWindow, int w, int h, dae::Camera* pCamera);
	~HardwareRasterizer();

	HardwareRasterizer(const HardwareRasterizer&) = delete;
	HardwareRasterizer(HardwareRasterizer&&) = delete;
	HardwareRasterizer& operator=(const HardwareRasterizer&) = delete;
	HardwareRasterizer& operator=(HardwareRasterizer&&) = delete;
	HRESULT InitializeDirectX(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	void Update(const dae::Timer* pTimer);
	void Render() const;
	void ChangeFilter();

private:
	

	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Resource* m_pRenderTargetBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	IDXGISwapChain* m_pSwapChain;
	//...
	Effect* m_pEffect;
	dae::Camera* m_pCamera{};
	SDL_Window* m_pWindow;
	int m_Width;
	int m_Height;
	Mesh* m_pVehicle;
};

