#include "Walnut/Application.h"

#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

#include "Walnut/Timer.h"
#include "Walnut/Random.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer(): m_Camera(45.0f,0.1f,100.0f) 
	{
		Material& pinksphere = m_Scene.Materials.emplace_back();
		pinksphere.Albedo = { 1.0f,0.0f,1.0f };
		pinksphere.Roughness = 0.0f;

		Material& bluesphere = m_Scene.Materials.emplace_back();
		bluesphere.Albedo = { 0.2f,0.3f,1.0f };
		bluesphere.Roughness = 0.1f;
		{
			Sphere sphere;
			sphere.Position = { 0.0f,0.0f,0.0f };
			sphere.Radius = 0.5f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 1.0f,0.0f,-5.0f };
			sphere.Radius = 1.5f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
	}
	virtual void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

			ImGui::Separator();
			ImGui::PopID();
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			Material& material = m_Scene.Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metalic", &material.Metalic, 0.05f, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(),(float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);
		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Scene m_Scene;
	Camera m_Camera;
	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}