#include "Renderer.h"


#include "Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& colour)
	{
		uint8_t r = (uint8_t)(colour.r * 255.0f);
		uint8_t g = (uint8_t)(colour.g * 255.0f);
		uint8_t b = (uint8_t)(colour.b * 255.0f);
		uint8_t a = (uint8_t)(colour.a * 255.0f);

		uint32_t result = ((a << 24) | (b << 16) | (g << 8) | r);
		return result;
	}
}
void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//no resize necessary 
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
			glm::vec4 colour = TraceRay(scene, ray);
			colour = glm::clamp(colour, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(colour);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance
	if (scene.Spheres.size() == 0)
	{
		return glm::vec4(0, 0, 0, 1);
	}
	const Sphere* closestsphere = nullptr;
	float hitdistance = std::numeric_limits<float>::max();
	for (const Sphere& sphere : scene.Spheres)
	{
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
		float c = glm::dot(ray.Origin, ray.Origin) - sphere.Radius * sphere.Radius;

		// Quadratic forumula discriminant:
		// b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
		{
			continue;
		}

		// Quadratic formula:
		// (-b +- sqrt(discriminant)) / 2a

		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT < hitdistance)
		{
			hitdistance = closestT;
			closestsphere = &sphere;
		}
	}
	if (closestsphere == nullptr)
	{
		return glm::vec4(0, 0, 0, 1);
	}
	glm::vec3 origin = ray.Origin - closestsphere->Position;
	glm::vec3 hitpoint = origin + ray.Direction * hitdistance;
	glm::vec3 normal = glm::normalize(hitpoint);
	glm::vec3 lightdir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightintensity = glm::max(glm::dot(normal, -lightdir), 0.0f);
	glm::vec3 spherecolour = closestsphere->Albedo;
	spherecolour *= lightintensity;
	return glm::vec4(spherecolour, 1.0f);
}