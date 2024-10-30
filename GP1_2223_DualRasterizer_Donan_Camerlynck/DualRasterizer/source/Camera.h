#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"
namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}
		const float movementSpeed{ 20.f };
		const float rotationSpeed{ 1.f };

		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		float m_AspectRatio;

		float zFar{ 100.f };
		float zNear{ .1f };

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f }, float aspectRatio = 1.f)
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);
			origin = _origin;
			m_AspectRatio = aspectRatio;
		}

		void CalculateViewMatrix()
		{

			Matrix finalRotation = Matrix::CreateRotation({ totalPitch, totalYaw, 0 });
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
									
			invViewMatrix = Matrix::CreateLookAtLH(origin, forward, Vector3::UnitY);

			viewMatrix = Matrix::Inverse(invViewMatrix);
			right = viewMatrix.GetAxisX();
			up = viewMatrix.GetAxisY();
		}



		void CalculateProjectionMatrix()
		{					
			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, m_AspectRatio, zNear, zFar);			
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//...
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			//Keyboard Input
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += movementSpeed * deltaTime * forward;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin += movementSpeed * deltaTime * -right;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin += movementSpeed * deltaTime * -forward;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += movementSpeed * deltaTime * right;
			}

			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);


			if (mouseState & SDL_BUTTON(1))
			{

				if (mouseState & SDL_BUTTON(3))
				{
					origin += movementSpeed * deltaTime * up * -mouseY;
				}
				else
				{
					origin += movementSpeed * deltaTime * -mouseY * forward;
					totalYaw += rotationSpeed * deltaTime * mouseX;
				}
			}
			else if (mouseState & SDL_BUTTON(3))
			{
				totalYaw += rotationSpeed * deltaTime * mouseX;
				totalPitch -= rotationSpeed * deltaTime * mouseY;

			}
			//Update Matrices
			/*CalculateViewMatrix();
			CalculateProjectionMatrix();*/ //Try to optimize this - should only be called once or when fov/aspectRatio changes
			
			fov = tan((fovAngle * TO_RADIANS) / 2);
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}

		
	};
}
