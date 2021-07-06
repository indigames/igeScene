#ifndef __UI_MASK_H__
#define __UI_MASK_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN


class UIMask : public UIImage
{
public:
	UIMask(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 256.f, 256.f });
	virtual ~UIMask();

	//! Get component name
	virtual std::string getName() const override { return "UIMask"; }

	//! Update
	virtual void onUpdate(float dt) override;

	//! Render
	virtual void onRender() override;

	void setUseMask(bool value);
	bool isUseMask() const { return m_bUseMask; }

	//! Update property by key value
	virtual void setProperty(const std::string& key, const json& val) override;

protected:
	bool m_bUseMask;
	bool m_maskDirty;

	void updateMask();
	void updateChildMask(SceneObject* child);
	void onAddChild(EventContext* context);
	
protected:
	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;
};

NS_IGE_SCENE_END


#endif //__UI_SCROLL_VIEW_H__