#pragma once
struct Vertex
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

struct Vertex_Out
{
	dae::Vector4 Position{};
	dae::Vector2 Uv{};
	dae::Vector3 Normal{};
	dae::Vector3 Tangent{};
	dae::Vector3 ViewDirection{};
};