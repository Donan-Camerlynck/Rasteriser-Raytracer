#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1

			//method 1 (unoptimized 10-11 fps avg) (optimized ??fps)

			const Vector3 tc{sphere.origin - ray.origin};// T(origin ray) to C(center sphere)
			
			const float dp{ Vector3::Dot(tc, ray.direction) }; //T to P ( length center sphere projected on ray) 
			const float cp{ tc.SqrMagnitude() - dp*dp }; //squared length C to P
			if(cp >= sphere.radius * sphere.radius)
			{
				hitRecord.didHit = false;
				return false;
			}
			const float tca{ (sqrt(sphere.radius * sphere.radius - cp)) }; //distance between P and I(first hit)
			const float ti{ dp - tca }; // distance T and I
			
			if(ti > ray.min && ti < ray.max)
			{
				if (!ignoreHitRecord) 
				{
					hitRecord.didHit = true;
					hitRecord.origin = ray.origin + ray.direction * ti;
					hitRecord.t = ti;
					hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
					hitRecord.materialIndex = sphere.materialIndex;
				}
				return true;
			}
			
			return false;

			//method 2 (12-13 avg fps)

			//const float a{ Vector3::Dot(ray.direction, ray.direction) };
			//const float b{ Vector3::Dot(2 * ray.direction,ray.origin - sphere.origin) };
			//const float c{ Vector3::Dot(ray.origin - sphere.origin,ray.origin - sphere.origin) - Square(sphere.radius)};
			//const float discriminant{sqrt(b*b-4*a*c)};
			//

			//if(discriminant > 0)
			//{
			//	float t{ (-b - discriminant) / (2 * a) };
			//	if(t < ray.min)
			//	{
			//		t = (-b + discriminant) / (2 * a);
			//	}
			//	if(t > ray.min && t < ray.max)
			//	{
			//		hitRecord.didHit = true;
			//		hitRecord.origin = ray.origin + ray.direction * t;
			//		hitRecord.t = t;
			//		hitRecord.normal = (sphere.origin - hitRecord.origin).Normalized();
			//		hitRecord.materialIndex = sphere.materialIndex;
			//		return true;
			//	}
			//}
			//hitRecord.didHit = false;
			//return false;
			

			//assert(false && "No Implemented Yet!");
			
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			const float dotProduct{ Vector3::Dot(ray.direction, plane.normal) };

			if (dotProduct < 0) {
				float t{ Vector3::Dot(plane.origin - ray.origin, plane.normal) / dotProduct };
				if (t > ray.min && t < ray.max)
				{
					hitRecord.didHit = true;
					hitRecord.origin = ray.origin + ray.direction * t;
					hitRecord.t = t;
					hitRecord.normal = plane.normal;
					hitRecord.materialIndex = plane.materialIndex;
					return true;
				}
			}
			hitRecord.didHit = false;
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			
			const Vector3 a{ triangle.v1 - triangle.v0 };
			const Vector3 b{ triangle.v2 - triangle.v0 };
			Vector3 n = Vector3::Cross(a, b);
			n.Normalize();
			const float dotRayNormal = Vector3::Dot(n, ray.direction);
					

			switch (triangle.cullMode)
			{
			case TriangleCullMode::FrontFaceCulling:
				if(ignoreHitRecord)
				{
					if (dotRayNormal > 0.f)
					{
						return false;
					}
				}
				else if(dotRayNormal < 0.f)
				{
					return false;
				}
				break;
			case TriangleCullMode::BackFaceCulling:
				if (ignoreHitRecord)
				{
					if (dotRayNormal < 0.f)
					{
						return false;
					}
				}
				else if (dotRayNormal > 0.f)
				{
					return false;
				}
				break;
			
			}

			if(abs(dotRayNormal) < FLT_EPSILON)
			{
				return false;
			}

			
			const Vector3 l = triangle.v0 - ray.origin;
			const float t = Vector3::Dot(l, n) / dotRayNormal;

			if(t < ray.min || t > ray.max)
			{
				return false;
			}

			Vector3 intersection = ray.origin + t * ray.direction;

			
			const Vector3 cross0 = Vector3::Cross(triangle.v1 - triangle.v0, intersection - triangle.v0);
			const Vector3 cross1 = Vector3::Cross(triangle.v2 - triangle.v1, intersection - triangle.v1);
			const Vector3 cross2 = Vector3::Cross(triangle.v0 - triangle.v2, intersection - triangle.v2);

			if( Vector3::Dot(n, cross0) < 0.f)
			{
				return false;
			}
			if (Vector3::Dot(n, cross1) < 0.f)
			{
				return false;
			}
			if (Vector3::Dot(n, cross2) < 0.f)
			{
				return false;
			}
			

			if(!ignoreHitRecord)
			{
				hitRecord.t = t;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.origin = intersection;
				hitRecord.didHit = true;
				hitRecord.normal = triangle.normal;
			}

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion

#pragma region TriangleMesh SlabTest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			const float tx1{ (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x };
			const float tx2{ (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x };

			float tmin{ std::min(tx1, tx2) };
			float tmax{ std::max(tx1, tx2) };

			const float ty1{ (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y };
			const float ty2{ (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y };

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			const float tz1{ (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z };
			const float tz2{ (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z };

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			//assert(false && "No Implemented Yet!");

			if(!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}

			for (int i{}; i < static_cast<int>(mesh.transformedNormals.size()); ++i)
			{
				HitRecord tempRecord{};
				int v0Id{ mesh.indices[i * 3] };
				int v1Id{ mesh.indices[i * 3 + 1] };
				int v2Id{ mesh.indices[i * 3 + 2] };
				Triangle triangle{ mesh.transformedPositions[v0Id], mesh.transformedPositions[v1Id], mesh.transformedPositions[v2Id] };
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;

				if(HitTest_Triangle(triangle, ray, tempRecord, ignoreHitRecord))
				{
					if(ignoreHitRecord)
					{
						return true;
					}
					if(tempRecord.t < hitRecord.t)
					{
						(hitRecord = tempRecord);
					}
				}
			}

			return hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
		
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			if (light.type != LightType::Directional) {
				return { light.origin - origin };
			}
			//assert(false && "No Implemented Yet!");
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			ColorRGB Radiance;
			Radiance = light.color * (light.intensity / (light.origin - target).SqrMagnitude());
			
			//assert(false && "No Implemented Yet!");
			return {Radiance};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}