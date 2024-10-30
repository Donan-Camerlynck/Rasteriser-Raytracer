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

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}

		const float movementSpeed{ 20.f };
		const float rotationSpeed{ 1.f };
		Vector3 origin{};
		float fovAngle{90.f};
		float fov{};

		//Vector3 forward{0.266f, -0.453f, 0.860};
		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();
			Matrix onb{
			Vector3{ right},
			Vector3{ up},
			Vector3{forward},
			Vector3{ origin }
			};
			cameraToWorld = onb;

			return {onb};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			//Keyboard Input
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				origin += movementSpeed * deltaTime * forward;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin += movementSpeed * deltaTime * - right;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin += movementSpeed * deltaTime *  - forward;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += movementSpeed * deltaTime * right;
			}


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			
			if (mouseState & SDL_BUTTON(1))
			{
				
				if(mouseState & SDL_BUTTON(3) )
				{
					origin += movementSpeed * deltaTime * up * -mouseY;
				}
				else
				{
					origin += movementSpeed * deltaTime * -mouseY * forward;
					totalYaw += rotationSpeed * deltaTime * mouseX;
				}				
			}
			else if(mouseState & SDL_BUTTON(3))
			{
				totalYaw += rotationSpeed * deltaTime * mouseX;
				totalPitch -= rotationSpeed * deltaTime * mouseY;
				
			}
			
			Matrix finalRotation = Matrix::CreateRotation({ totalPitch, totalYaw, 0 });
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			//todo: W2
			fov = tan((fovAngle*TO_RADIANS)/2);
			//assert(false && "Not Implemented Yet");
		}
	};
}
