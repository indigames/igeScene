#ifndef __BITMAP_FONT_H__
#define __BITMAP_FONT_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "utils/Serialize.h"

using namespace pyxie;

namespace ige::scene
{
	struct BitmapGlyph
	{
		uint32_t ID;
		std::string Name;
		wchar_t Unicode;
		float x;
		float y;
		float w;
		float h;
		float xoffset;
		float yoffset;
		float xadvance;
	};

	class BitmapFont
	{
	public:
		BitmapFont(std::string Name);
		~BitmapFont();
		
		std::shared_ptr<struct BitmapGlyph> getGlyph(int index);
		void addGlyph(std::shared_ptr<struct BitmapGlyph> glyph);

		void To_json(json& j);
		void From_json(const json& j);

		void setName(std::string Name) { m_name = Name;  }
		std::string getName() const { return m_name; }
		int getGlyphCount() const { return m_glyphs.size();  }

		int getSize() { return m_size; }
		void setSize(int value) { m_size = value;  }

		int getBaseSize() { return m_base; }
		void setBaseSize(int value) { m_base = value; }

		int getWidth() { return m_width; }
		void setWidth(int value) { m_width = value; }
		int getHeigth() { return m_height; }
		void setHeight(int value) { m_height = value; }

	protected:
		void clear();
		void Glyph_To_json(json& j, const std::shared_ptr<struct BitmapGlyph>& glyph);
		void Glyph_From_json(const json& j, std::shared_ptr<struct BitmapGlyph>& glyph);

	protected:
		std::string m_name;
		std::vector<std::shared_ptr<struct BitmapGlyph>> m_glyphs;

		int m_size = 0;
		int m_base = 0;
		int m_width = 0;
		int m_height = 0;
	};
}

#endif //__BITMAP_FONT_H__