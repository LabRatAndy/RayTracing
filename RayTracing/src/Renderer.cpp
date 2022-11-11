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

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(),(float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f;	//ensure coord is  in range -1.0 to 1.0
			glm::vec4 colour = PerPixel(coord);
			colour = glm::clamp(colour, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(colour);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2  coord)
{
	glm::vec3 rayorigin(0.0f, 0.0f, 2.0f);
	glm::vec3 raydirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	float a = glm::dot(raydirection, raydirection);
	float b = 2.0f * glm::dot(rayorigin, raydirection);
	float c = glm::dot(rayorigin, rayorigin) - radius * radius;

	// Quadratic forumula discriminant:
	// b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
	{
		return glm::vec4(0, 0, 0, 1);
	}
	
	// Quadratic formula:
	// (-b +- sqrt(discriminant)) / 2a

	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 hitpoint = rayorigin + raydirection * closestT;
	glm::vec3 normal = glm::normalize(hitpoint);
	glm::vec3 lightdir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightintensity = glm::max(glm::dot(normal, -lightdir), 0.0f);
	glm::vec3 spherecolour(1, 0, 1);
	spherecolour *= lightintensity;
	return glm::vec4(spherecolour, 1.0f);
}