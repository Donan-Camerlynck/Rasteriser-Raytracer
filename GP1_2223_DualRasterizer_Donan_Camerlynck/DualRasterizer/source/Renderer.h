#pragma once
#include "Camera.h"
#include "Effect.h"
#include "HardwareRasterizer.h"
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();
		

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		void ChangeFilter();
		void ChangeRenderMethod();
		void ChangeNormalMap();
		void ChangeDepthBufferVisualisation();
		void ChangeBoundingBoxVisualisation();

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsDXInitialized{ false };
		

		Camera m_pCamera{};

		enum class RenderMode
		{
			Hardware, Software
		};

		HardwareRasterizer* m_pHardwareRasterizer;

		RenderMode m_CurrentRenderMode;

	};
}
