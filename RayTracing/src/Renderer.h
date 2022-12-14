#pragma once
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() = default;
	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

private:
	struct HitPayLoad
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;
		int ObjectIndex;
	};

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	HitPayLoad TraceRay(const Ray& ray);
	HitPayLoad ClosestHit(const Ray& ray, float hitdistance, int objectindex);
	HitPayLoad Miss(const Ray& ray);
};