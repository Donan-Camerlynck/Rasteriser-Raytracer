#pragma once
#include <cassert>
#include <xlocale>

#include "Math.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			return {cd * kd / PI};
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			return { cd * kd / PI };
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			Vector3 reflect = l - 2 * (Vector3::Dot(n, l) * n);
			float cosine = std::max(0.f, Vector3::Dot(reflect, -v));
			float phongValue = ks * pow(cosine, exp);
			return { ColorRGB{phongValue, phongValue, phongValue} };
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			float dot = Vector3::Dot(h, v);
			
			float part1 = 1 - dot;
			ColorRGB fRGB = f0 + (ColorRGB{1.f,1.f,1.f} - f0) * powf(part1, 5);

			return {fRGB};
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");

			float alpha = pow(roughness, 4.f);
			float denominatorPart = ((Vector3::Dot(n, h) * Vector3::Dot(n, h) * (alpha - 1) + 1) );

			return { (alpha/ (denominatorPart * denominatorPart * static_cast<float>(M_PI)))};
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			const float alpha = roughness * roughness;
			const float k = (alpha + 1) * (alpha + 1) / 8;
			float dot = Vector3::Dot(n, v);
			if(dot < 0.f)
			{
				dot = 0.f;
			}
			return { dot / (dot * (1 - k) + k)};
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			/*const float alpha = roughness * roughness;
			const float k = (alpha + 1) * (alpha + 1) / 8;*/
			const float masking = GeometryFunction_SchlickGGX(n,v,roughness);
			const float shadowing = GeometryFunction_SchlickGGX(n, l, roughness);
			return { masking * shadowing };
		}

	}
}