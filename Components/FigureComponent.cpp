#include "FigureComponent.h"

namespace ige::scene
{
	FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner): Component(owner) {}

	FigureComponent::~FigureComponent() {}


	bool FigureComponent::loadFigure(const std::string& path)
	{
		m_figure = std::make_shared<pyxie::pyxieFigure>(path.c_str());
		if (m_figure)
		{
			m_figure->Build();
			m_figure->Initialize();
			return true;
		}
		return false;
	}

	void FigureComponent::onUpdate(float dt)
	{
		if (m_figure)
		{
			m_figure->Step(dt);
			m_figure->Render();
		}
	}

	//! Serialize
	void FigureComponent::to_json(json& j, const Component& obj)
	{

	}

	//! Deserialize
	void FigureComponent::from_json(const json& j, Component& obj)
	{

	}
}
