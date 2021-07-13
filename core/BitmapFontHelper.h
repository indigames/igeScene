#ifndef __FONT_BITMAP_H__
#define __FONT_BITMAP_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"
#include "utils/Singleton.h"

#include "core/BitmapFont.h"

#include <stb_truetype.h>

using namespace pyxie;

namespace ige::scene
{

#define HASH_LUT_SIZE 256
#define MAX_ROWS 128
#define VERT_COUNT (6*128)

	struct ige_bitmap_font
	{
		uint32_t idx;
		int type;
		stbtt_fontinfo font;
		unsigned char* data;
		struct ige_glyph* glyphs;
		int lut[HASH_LUT_SIZE];
		int nglyphs;
		float ascender;
		float descender;
		float lineh;
		int tw, th;
		float itw, ith;
	};

	struct ige_texture_row
	{
		short x, y, h;
	};

	struct ige_texture
	{
		uint32_t id;
		pyxieTexture* texture;
		// TODO: replace rows with pointer
		struct ige_texture_row rows[MAX_ROWS];
		int nrows;
		float verts[4 * VERT_COUNT];
		int nverts;
		struct ige_texture* next;
	};

	struct ige_glyph
	{
		unsigned int codepoint;
		short size;
		struct ige_texture* texture;
		int x0, y0, x1, y1;
		float xadv, xoff, yoff;
		int next;
	};

	struct ige_quad
	{
		float x0, y0, s0, t0;
		float x1, y1, s1, t1;
	};

	class BitmapFontHelper : public Singleton<BitmapFontHelper>
	{
	public:
		BitmapFontHelper();
		virtual ~BitmapFontHelper();

		uint32_t createBitmapFont(const std::string& path);

		uint32_t createBitmapFont(std::shared_ptr<BitmapFont>& font);

		uint32_t createBitmapFont(const std::string& path, int ascent, int descent, int line_gap);

		uint32_t addGlyphForCodepoint(uint32_t idx, unsigned int codepoint,
			short size, short base, int x, int y, int w, int h,
			float xoffset, float yoffset, float xadvance);

		uint32_t addGlyphForChar(uint32_t idx, const char* s,
			short size, short base, int x, int y, int w, int h,
			float xoffset, float yoffset, float xadvance);

		EditableFigure* createText(const char* word, const std::string& fontPath, int fontsize);

		void loadBitmapFromPath(std::string path, std::shared_ptr<BitmapFont> &font);
	protected:
		std::string unique(const std::string& str);

		struct ige_glyph* getGlyph(struct ige_bitmap_font* fnt, unsigned int codepoint, short isize);
		int getQuad(struct ige_bitmap_font* fnt, struct ige_glyph* glyph, short isize, float* x, float* y, struct ige_quad* q);

	protected:
		
	};

}

#endif //__FONT_BITMAP_H__