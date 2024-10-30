#include "pch.h"
#include "Renderer.h"

#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
		m_pCamera.Initialize(45.f, { .0f,.0f,0.f }, static_cast<float>(m_Width) / m_Height );
		
		std::vector<Vertex> tempVertices;
		std::vector<uint32_t> tempIndices;

		Utils::ParseOBJ("resources/vehicle.obj", tempVertices, tempIndices);

		m_pHardwareRasterizer = new HardwareRasterizer{ pWindow, m_Width, m_Height, &m_pCamera };
		const HRESULT result = m_pHardwareRasterizer->InitializeDirectX(tempVertices, tempIndices);
		if (FAILED(result))
		{
			m_IsDXInitialized = false;
			std::cout << "DirectX Initialisation failed\n";
		}
		else
		{
			m_IsDXInitialized = true;
		}

		m_CurrentRenderMode = RenderMode::Hardware;

	}

	Renderer::~Renderer()
	{
		delete m_pHardwareRasterizer;
		m_pHardwareRasterizer = nullptr;
		
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera.Update(pTimer);
		m_pHardwareRasterizer->Update(pTimer);
	}


	void Renderer::Render() const
	{
		
		switch (m_CurrentRenderMode)
		{
		case dae::Renderer::RenderMode::Hardware:
			m_pHardwareRasterizer->Render();
			break;
		case dae::Renderer::RenderMode::Software:
			//m_pSoftwareRasterizer->Render();
			break;

		}
	}

	void Renderer::ChangeFilter()
	{
		if(m_CurrentRenderMode == RenderMode::Hardware)
		{
			m_pHardwareRasterizer->ChangeFilter();
		}
	}


	

	

}
