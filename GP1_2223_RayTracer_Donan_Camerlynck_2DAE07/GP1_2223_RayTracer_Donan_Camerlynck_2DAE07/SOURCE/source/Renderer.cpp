//External includes
#include "SDL.h"
#include "SDL_surface.h"

//#define ASYNC
#define PARALLEL_FOR
#include <future>
#include <ppl.h>


//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	
}



void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	camera.CalculateCameraToWorld();

	const float fovAngle = camera.fovAngle * TO_RADIANS;
	const float fov = tan(fovAngle / 2.f);

	const float aspectRatio = m_Width / static_cast<float>(m_Height);

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t numPixels = m_Width * m_Height;

#if defined(ASYNC)
	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t numPixelsPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;
	uint32_t currPixelIndex = 0;

	for(uint32_t coreId{}; coreId < numCores; ++coreId)
	{
		uint32_t taskSize = numPixelsPerTask;
		if(numUnassignedPixels > 0)
		{
			++taskSize;
			--numUnassignedPixels;
		}

		async_futures.push_back(std::async(std::launch::async, [=, this]
			{
				const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
				for (uint32_t pixelIndex{ currPixelIndex }; pixelIndex < pixelIndexEnd; ++pixelIndex)
				{
					RenderPixel(pScene, pixelIndex, fov, aspectRatio, camera, lights, materials);
				}
			}));

			currPixelIndex += taskSize;
	}

	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}
#elif defined(PARALLEL_FOR)

	concurrency::parallel_for(0u, numPixels, [=, this](int i)
		{
			RenderPixel(pScene, i, fov, aspectRatio, camera, lights, materials);
		});

#else
	for(uint32_t i {}; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, fov, aspectRatio, pScene->GetCamera(), lights, materials);
	}
#endif
	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera,
						const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	
	ColorRGB finalColor{};
	HitRecord closestHit{};
	Vector3 rayDirection = { cx , cy , 1};
	rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
	rayDirection.Normalize();

	Ray viewRay{ camera.origin, rayDirection };


	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		//finalColor = materials[closestHit.materialIndex]->Shade();

		for (auto& light : lights)
		{
			Vector3 directionToLight = LightUtils::GetDirectionToLight(light, closestHit.origin);
			Vector3 directionToLightNormalized = directionToLight.Normalized();
			Ray lightRay{ closestHit.origin + closestHit.normal * 0.001f, directionToLightNormalized, 0.0001f, directionToLight.Magnitude() };
			if (m_ShadowsEnabled)
			{
				if (pScene->DoesHit(lightRay))
				{
					continue;
				}
			}
			const float cosineLawDot = Vector3::Dot(closestHit.normal, directionToLightNormalized);
			switch (m_CurrentLightingMode)
			{
			case LightingMode::ObservedArea:
				if (cosineLawDot > 0)
				{
					finalColor += ColorRGB{ cosineLawDot, cosineLawDot, cosineLawDot };
				}
				break;
			case LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin);
				break;
			case LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLightNormalized, rayDirection);
				break;
			case LightingMode::Combined:
				if (cosineLawDot > 0)
				{
					finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, directionToLightNormalized, rayDirection) * cosineLawDot;
				}
				break;
			}
		}
	}

	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[pixelIndex] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}


bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}



void Renderer::CycleLightingMode()
{
	switch(m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case LightingMode::BRDF:
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	}
}


