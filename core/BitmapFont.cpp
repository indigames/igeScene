#include "core/BitmapFont.h"


NS_IGE_SCENE_BEGIN

BitmapFont::BitmapFont(std::string name) : m_name(name)
{
	
}

BitmapFont::~BitmapFont()
{
	clear();
}

void BitmapFont::clear()
{
	int num = m_glyphs.size();
	if (num > 0) 
	{
		for (int i = 0; i < num; i++)
		{
			m_glyphs[i] = nullptr;
		}
		m_glyphs.clear();
	}
}

void BitmapFont::To_json(json& j)
{	
	int num = m_glyphs.size();
	j["size"] = getSize();
	j["base"] = getBaseSize();
	j["numglyph"] = num;
	for (int i = 0; i < num; i++)
	{
		json jd;
		Glyph_To_json(jd, m_glyphs[i]);
		j[std::to_string(i)] = jd;
	}
}

void BitmapFont::From_json(const json& j)
{
	clear();
	int num = j.value("numglyph", 0);
	setSize(j.value("size", 0));
	setBaseSize(j.value("base", 0));
	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{
			json jd = j.at(std::to_string(i));
			if (!jd.is_null()) {
				auto glyph = std::make_shared<BitmapGlyph>();
				Glyph_From_json(jd, glyph);
				this->addGlyph(glyph);
			}
		}
	}
}

std::shared_ptr<struct BitmapGlyph> BitmapFont::getGlyph(int index)
{
	if (index >= m_glyphs.size() || index < 0) return nullptr;
	return m_glyphs[index];
}

void BitmapFont::addGlyph(std::shared_ptr<struct BitmapGlyph> glyph)
{
	if (glyph == nullptr) return;
	m_glyphs.push_back(glyph);
}

void BitmapFont::Glyph_To_json(json& j,const std::shared_ptr<struct BitmapGlyph> &glyph)
{
	j["id"] = glyph->ID;
	j["name"] = glyph->Name;
	j["unicode"] = glyph->Unicode;
	j["x"] = glyph->x;
	j["y"] = glyph->y;
	j["w"] = glyph->w;
	j["h"] = glyph->h;
	j["xoffset"] = glyph->xoffset;
	j["yoffset"] = glyph->yoffset;
	j["xadvance"] = glyph->xadvance;
}

void BitmapFont::Glyph_From_json(const json& j, std::shared_ptr<struct BitmapGlyph>& glyph)
{
	glyph->ID = j.value("id", 0);
	std::string name = j.at("name");
	glyph->Name = name.c_str();
	glyph->Unicode = j.value("unicode", L'\000');
	glyph->x = j.value("x", 0);
	glyph->y = j.value("y", 0);
	glyph->w = j.value("w", 0);
	glyph->h = j.value("h", 0);
	glyph->xoffset = j.value("xoffset", 0);
	glyph->yoffset = j.value("yoffset", 0);
	glyph->xadvance = j.value("xadvance", 0);
}

NS_IGE_SCENE_END