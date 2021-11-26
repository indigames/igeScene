#include "core/BitmapFontHelper.h"
#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

#define TTFONT_FILE 1
#define TTFONT_MEM  2
#define BMFONT      3

#define STH_ESUCCESS 0

// error opening file
#define STH_EFILEIO -1

// error initializing truetype font
#define STH_ETTFINIT -2

// invalid argument
#define STH_EINVAL -3

// not enough memory
#define STH_ENOMEM -4

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

#define STH_GL_TEXTYPE   GL_RED

#define VERT_STRIDE (sizeof(float)*4)

//int FontBitmap::idx = 1;

static const unsigned char utf8d[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
	8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
	0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
	0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
	0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
	1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
	1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
	1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

static unsigned int decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
{
	unsigned int type = utf8d[byte];
	*codep = (*state != UTF8_ACCEPT) ?
		(byte & 0x3fu) | (*codep << 6) :
		(0xff >> type) & (byte);
	*state = utf8d[256 + *state * 16 + type];
	return *state;
}

static unsigned int hashint(unsigned int a)
{
	a += ~(a << 15);
	a ^= (a >> 10);
	a += (a << 3);
	a ^= (a >> 6);
	a += ~(a << 11);
	a ^= (a >> 16);
	return a;
}

inline int32_t ConvertUtf16ToUtf8(int8_t* dst, int32_t dst_size, const int16_t* src)
{
	int32_t cnt = 0;
	const int16_t* wp = src;
	int8_t* cp = dst;

	if (dst_size == 0)
		return 0;

	dst_size -= 3;

	for (cnt = 0; cnt < dst_size;)
	{
		int16_t wc = *wp++;
		if (wc == 0)
		{
			break;
		}
		if ((wc & ~0x7f) == 0)
		{
			*cp++ = wc & 0x7f;
			cnt += 1;
		}
		else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >> 6) & 0x1f) | 0xc0;
			*cp++ = ((wc)&0x3f) | 0x80;
			cnt += 2;
		}
		else
		{
			*cp++ = ((wc >> 12) & 0xf) | 0xe0;
			*cp++ = ((wc >> 6) & 0x3f) | 0x80;
			*cp++ = ((wc)&0x3f) | 0x80;
			cnt += 3;
		}
	}
	*cp = '\0';
	return cnt;
}

static std::string utf16toUtf8(const std::wstring& wstr)
{
    char utf8_str[2048] = {0};
    ConvertUtf16ToUtf8((int8_t*)utf8_str, 2048, (const int16_t*)wstr.c_str());
    return std::string(utf8_str);
}

static float* setv(float* v, float x, float y, float s, float t)
{
	v[0] = x;
	v[1] = y;
	v[2] = s;
	v[3] = t;
	return v + 4;
}

static std::map<uint32_t, ige_bitmap_font*> fonts;
static std::map<uint32_t, ige_texture*> textures;

BitmapFontHelper::BitmapFontHelper()
{

}

BitmapFontHelper::~BitmapFontHelper()
{

}

std::string BitmapFontHelper::unique(const std::string& str)
{
	static int uniqueNumber = 249999; // avoid conflict with Python
	uniqueNumber++;
	return str + std::to_string(uniqueNumber);
}

uint32_t BitmapFontHelper::createBitmapFont(const std::string& path, int width, int height, int ascent, int descent, int line_gap)
{
	int ret, i, fh;
	struct ige_bitmap_font* fnt = NULL;
	struct ige_texture* texture = NULL;
	pyxieTexture* m_texture = NULL;

	uint32_t hash = pyxie::GenerateNameHash(path.c_str());
	auto it = fonts.find(hash);
	if (it == fonts.end())
	{
		texture = (struct ige_texture*)malloc(sizeof(struct ige_texture));
		if (texture == NULL)
			return STH_ENOMEM;
		memset(texture, 0, sizeof(struct ige_texture));
		texture->id = hash;

		fnt = (struct ige_bitmap_font*)malloc(sizeof(struct ige_bitmap_font));
		if (fnt == NULL)
		{
			goto error;
		}
		memset(fnt, 0, sizeof(struct ige_bitmap_font));

		// Init hash lookup.
		for (i = 0; i < HASH_LUT_SIZE; ++i) fnt->lut[i] = -1;

		// Store normalized line height. The real line height is got
		// by multiplying the lineh by font size.
		/*fh = ascent - descent;
		fnt->ascender = (float)ascent / (float)fh;
		fnt->descender = (float)descent / (float)fh;
		fnt->lineh = (float)(fh + line_gap) / (float)fh;*/

		m_texture = ResourceCreator::Instance().NewTexture(path.c_str());
		m_texture->WaitInitialize();
		m_texture->WaitBuild();
		texture->texture = m_texture;
		
		fonts[hash] = fnt;
		textures[hash] = texture;

		auto cachew = width;
		auto cacheh = height;
		
		fnt->tw = cachew;
		fnt->th = cacheh;
		fnt->itw = 1.0f / cachew;
		fnt->ith = 1.0f / cacheh;
		fnt->idx = hash;
		fnt->type = BMFONT;
	}
	return hash;

error:
	if (fnt) free(fnt);
	if (m_texture) m_texture->DecReference();
	if (texture) free(texture);
	return 0;
}

uint32_t BitmapFontHelper::addGlyphForCodepoint(uint32_t idx, unsigned int codepoint,
	short size, short base, int x, int y, int w, int h,
	float xoffset, float yoffset, float xadvance)
{
	struct ige_texture* texture = NULL;
	struct ige_bitmap_font* fnt = NULL;
	struct ige_glyph* glyph = NULL;

	fnt = fonts[idx];
	if (fnt == NULL)
		return STH_EINVAL;
	if (fnt->type != BMFONT)
		return STH_EINVAL;
	
	texture = textures[idx];
	if (texture == NULL)
	{
		// Create new texture
		texture = (struct ige_texture*)malloc(sizeof(struct ige_texture));
		if (texture == NULL)
			return STH_ENOMEM;
		memset(texture, 0, sizeof(struct ige_texture));
		texture->id = idx;
		textures[idx] = texture;
	}

	// Alloc space for new glyph.
	fnt->nglyphs++;
	fnt->glyphs = (struct ige_glyph*)realloc(fnt->glyphs, fnt->nglyphs * sizeof(struct ige_glyph)); /* @rlyeh: explicit cast needed in C++ */
	if (!fnt->glyphs)
		return STH_ENOMEM;

	// Init glyph.
	glyph = &fnt->glyphs[fnt->nglyphs - 1];
	memset(glyph, 0, sizeof(struct ige_glyph));
	glyph->codepoint = codepoint;
	glyph->size = size;
	glyph->texture = texture;
	glyph->x0 = x;
	glyph->y0 = y;
	glyph->x1 = glyph->x0 + w;
	glyph->y1 = glyph->y0 + h;
	glyph->xoff = xoffset;
	glyph->yoff = yoffset - base;
	glyph->xadv = xadvance;

	// Find code point and size.
	h = hashint(codepoint) & (HASH_LUT_SIZE - 1);
	// Insert char to hash lookup.
	glyph->next = fnt->lut[h];
	fnt->lut[h] = fnt->nglyphs - 1;

	return STH_ESUCCESS;
}

uint32_t BitmapFontHelper::addGlyphForChar(uint32_t idx, const char* s,
	short size, short base, int x, int y, int w, int h,
	float xoffset, float yoffset, float xadvance)
{
	unsigned int codepoint;
	unsigned int state = 0;

	for (; *s; ++s)
	{
		if (!decutf8(&state, &codepoint, *(unsigned char*)s))
			break;
	}
	if (state != UTF8_ACCEPT)
		return STH_EINVAL;

	return addGlyphForCodepoint(idx, codepoint, size, base, x, y, w, h, xoffset, yoffset, xadvance);
}

struct ige_glyph* BitmapFontHelper::getGlyph(struct ige_bitmap_font* fnt, unsigned int codepoint, short isize)
{
	int i, g, advance, lsb, x0, y0, x1, y1, gw, gh;
	float scale;
	struct ige_texture* texture = NULL;
	struct ige_glyph* glyph = NULL;
	unsigned char* bmp = NULL;
	unsigned int h;
	float size = isize / 10.0f;
	int rh;
	struct ige_texture_row* br = NULL;

	// Find code point and size.
	h = hashint(codepoint) & (HASH_LUT_SIZE - 1);
	i = fnt->lut[h];
	while (i != -1)
	{
		if (fnt->glyphs[i].codepoint == codepoint && (fnt->type == BMFONT || fnt->glyphs[i].size == isize))
			return &fnt->glyphs[i];
		i = fnt->glyphs[i].next;
	}
	// Could not find glyph.
	// For bitmap fonts: ignore this glyph.
	if (fnt->type == BMFONT)
		return 0;
	return 0;

error:
	if (texture)
		free(texture);
	return 0;
}

int BitmapFontHelper::getQuad(struct ige_bitmap_font* fnt, struct ige_glyph* glyph, short isize, float* x, float* y, struct ige_quad* q)
{
	float rx, ry;
	float scale = 1.0f;

	if (fnt->type == BMFONT) scale = (float)isize / (glyph->size * 10.0f);
	
	/*rx = floorf(*x + scale * glyph->xoff);
	ry = floorf(*y - scale * glyph->yoff);*/
	rx = (*x + scale * glyph->xoff);
	ry = (*y - scale * glyph->yoff);

	q->x0 = rx;
	q->y0 = ry;
	q->x1 = rx + scale * (float)(glyph->x1 - glyph->x0);
	q->y1 = ry - scale * (float)(glyph->y1 - glyph->y0);
	
	q->s0 = (glyph->x0) * fnt->itw;
	q->t0 = (glyph->y0) * fnt->ith;
	q->s1 = (glyph->x1) * fnt->itw;
	q->t1 = (glyph->y1) * fnt->ith;
	
	if(glyph->xadv != 0)
		*x += scale * glyph->xadv;
	else 
		*x += scale * (glyph->x1 - glyph->x0);

	return 1;
}

EditableFigure* BitmapFontHelper::createText(const char* s, const std::string& fontPath, int size, int& outW, int& outH)
{

	auto fsPath = fs::path(fontPath);
	if (fsPath.extension().string() != ".pybm")
		return 0;

	auto ext = fsPath.extension();
	if (ext.string() != ".pyxi")
	{
		fsPath = fsPath.replace_extension(".pyxi");
	}
	std::string imgPath = fsPath.string();

	uint32_t hash = pyxie::GenerateNameHash(imgPath.c_str());
	auto it = fonts.find(hash);
	if (it == fonts.end()) return nullptr;


	unsigned int codepoint;
	struct ige_glyph* glyph = NULL;
	struct ige_texture* texture = NULL;
	unsigned int state = 0;
	struct ige_quad q;
	short isize = (short)(size * 10.0f);
	float* v;
	struct ige_bitmap_font* fnt = NULL;
	float x = 0, y = 0;

	fnt = fonts[hash];
	if (fnt == NULL)
		return nullptr;
	if (fnt->type != BMFONT && !fnt->data)
		return nullptr;
	uint32_t s_len = strlen(s);
	uint32_t nverts = s_len * 4;
	uint32_t indiceCount = nverts * 3 / 2;
	
	std::vector<float> points;
	std::vector<uint32_t> trianglesIndices;
	std::vector<float> uvs;

	int fOffset = 0;

	//float w = 0, h = 0;
	int len = 0;

	for (int i = 0; i < indiceCount;) {
	
		trianglesIndices.push_back(fOffset + 0);
		trianglesIndices.push_back(fOffset + 2);
		trianglesIndices.push_back(fOffset + 1);

		trianglesIndices.push_back(fOffset + 1);
		trianglesIndices.push_back(fOffset + 2);
		trianglesIndices.push_back(fOffset + 3);

		i += 6;
		fOffset += 4;
	}

	float ix = 0, ax = 0, iy = 0, ay = 0;
	for (; *s; ++s) 
	{
		if (decutf8(&state, &codepoint, *(unsigned char*)s))
			continue;
		glyph = getGlyph(fnt, codepoint, isize);
		if (!glyph)
			continue;
		texture = glyph->texture;


		if (!getQuad(fnt, glyph, isize, &x, &y, &q))
			continue;
		float h = q.y0 - q.y1;
		points.push_back(q.x0);
		points.push_back(q.y0 - h);
		points.push_back(0);

		points.push_back(q.x1);
		points.push_back(q.y0 - h);
		points.push_back(0);

		points.push_back(q.x0);
		points.push_back(q.y1 - h);
		points.push_back(0);

		points.push_back(q.x1);
		points.push_back(q.y1 - h);
		points.push_back(0);

		uvs.push_back(q.s0);
		uvs.push_back(1 - q.t0);

		uvs.push_back(q.s1);
		uvs.push_back(1 - q.t0);

		uvs.push_back(q.s0);
		uvs.push_back(1 - q.t1);

		uvs.push_back(q.s1);
		uvs.push_back(1 - q.t1);

		if (len == 0) {
			if (q.x0 < q.x1) { ix = q.x0; ax = q.x1; }
			else { ix = q.x1; ax = q.x0; }
			if (q.y0 < q.y1) { iy = q.y0; ay = q.y1; }
			else { iy = q.y1; ay = q.y0; }
		}
		else {
			if (q.x0 < q.x1) {
				ix = ix > q.x0 ? q.x0 : ix;
				ax = ax < q.x1 ? q.x1 : ax;
			}
			else {
				ix = ix > q.x1 ? q.x1 : ix;
				ax = ax < q.x0 ? q.x0 : ax;
			}
			if (q.y0 < q.y1) {
				iy = iy > q.x0 ? q.y0 : iy;
				ax = ay < q.y1 ? q.y1 : ay;
			}
			else {
				iy = iy > q.y1 ? q.y1 : iy;
				ay = ay < q.y0 ? q.y0 : ay;
			}
		}
		len++;
	}

	if (texture == nullptr) return nullptr;

	float hw = std::abs(ax - ix) * 0.5f;
	float hh = std::abs(ay - iy) * 0.5f;
	outW = hw * 2;
	outH = hh * 2;
	fOffset = 0;
	int pointSize = points.size();
	for (int i = 0; i < len; i++)
	{
		if (fOffset >= pointSize) break;

		points[fOffset] -= hw;
		points[fOffset + 1] += hh;

		points[fOffset + 3] -= hw;
		points[fOffset + 4] += hh;

		points[fOffset + 6] -= hw;
		points[fOffset + 7] += hh;

		points[fOffset + 9] -= hw;
		points[fOffset + 10] += hh;
		
		fOffset += 12;
	}

	//Create Figure
	auto shader = ResourceCreator::Instance().NewShaderDescriptor();
	shader->SetColorTexture(true);
	shader->SetBoneCondition(1, 1);
	shader->DiscardColorMapRGB(true);

	auto efig = GraphicsHelper::getInstance()->createMesh(points, trianglesIndices, texture->texture, uvs, shader);
	return efig;
}

void BitmapFontHelper::loadBitmapFromPath(std::string path, std::shared_ptr<BitmapFont> &font)
{
	if (path.empty() || font == nullptr)
		return;

	auto fsPath = fs::path(path);
	if (fsPath.extension().string() != ".pybm")
		return;

	std::ifstream file(fsPath);
	if (!file.is_open())
		return;

	json jObj;
	file >> jObj;

	auto ext = fsPath.extension();
	if (ext.string() != ".pyxi")
	{
		fsPath = fsPath.replace_extension(".pyxi");
	}
	std::string imgPath = fsPath.string();
	font->From_json(jObj);
	font->setName(imgPath);	
}

uint32_t BitmapFontHelper::createBitmapFont(std::shared_ptr<BitmapFont>& font)
{
	if (font == nullptr) return 0;
	auto name = font->getName();
	uint32_t id = createBitmapFont(name, font->getWidth(), font->getHeigth(), 0, 0, 0);
	int size = font->getGlyphCount();
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			auto glyph = font->getGlyph(i);
			const auto* wc = &glyph->Unicode;
			auto c = utf16toUtf8(wc);
			addGlyphForChar(id, c.c_str(), font->getSize(), font->getBaseSize(), (int)glyph->x, (int)glyph->y, (int)glyph->w, (int)glyph->h,
				glyph->xoffset, glyph->yoffset, glyph->xadvance);
		}
	}

	return id;
}

uint32_t BitmapFontHelper::createBitmapFont(const std::string& path)
{
	if (path.empty()) return 0;

	auto fsPath = fs::path(path);
	if (fsPath.extension().string() != ".pybm")
		return 0;

	auto ext = fsPath.extension();
	if (ext.string() != ".pyxi")
	{
		fsPath = fsPath.replace_extension(".pyxi");
	}
	std::string imgPath = fsPath.string();

	uint32_t hash = pyxie::GenerateNameHash(imgPath.c_str());
	auto it = fonts.find(hash);
	if (it == fonts.end())
	{
		auto bitmapfont = std::make_shared<BitmapFont>("");
		loadBitmapFromPath(path, bitmapfont);
		hash = createBitmapFont(bitmapfont);
	}
	return hash;
}

NS_IGE_SCENE_END