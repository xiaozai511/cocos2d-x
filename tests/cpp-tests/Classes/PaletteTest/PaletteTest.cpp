#include "PaletteTest.h"

const char* cc_palette_vert = R"(
	attribute vec4 a_position;
	attribute vec2 a_texCoord;

	#ifdef GL_ES
	varying mediump vec2 v_texCoord;
	#else
	varying vec2 v_texCoord;
	#endif

	void main()
	{
		gl_Position = CC_MVPMatrix * a_position;
		v_texCoord = a_texCoord;
	}
)";

const char* cc_palette_frag = R"(
	#ifdef GL_ES
	precision highp float;
  	#endif

	varying vec4 v_fragmentColor;
	varying vec2 v_texCoord;

	//uniform sampler2D CC_Texture0;
	//uniform sampler2D CC_Texture1;

	// palette: 256*16
	vec4 getPaletteColor16(vec4 texColor)
	{
		vec4 paletteColor = texture2D(CC_Texture1, vec2(clamp(texColor.r / 16.0, 0.001, 0.061) + floor(texColor.b * 16.0) / 16.0, texColor.g));
		paletteColor *= texColor.a;
		return paletteColor;
	}

	// palette: 1024*32
	vec4 getPaletteColor32(vec4 texColor)
	{
		vec4 paletteColor = texture2D(CC_Texture1, vec2(clamp(texColor.r / 32.0, 0.001, 0.03) + floor(texColor.b * 32.0) / 32.0, texColor.g));
		paletteColor *= texColor.a;
		return paletteColor;
	}

	// palette: 512*512(4096*64)
	vec4 getPaletteColor64(vec4 texColor)
	{
		float index = floor(texColor.b * 64.0);
		float u = clamp(texColor.r / 8.0, 0.001, 0.124) + mod(index, 8.0) / 8.0;
		float v = clamp(texColor.g / 8.0, 0.001, 0.124) + floor(index / 8.0) / 8.0;
		vec4 paletteColor = texture2D(CC_Texture1, vec2(u, v));
		paletteColor *= texColor.a;
		return paletteColor;
	}


	void main()
	{
		vec4 texColor = texture2D(CC_Texture0, v_texCoord);// * v_fragmentColor;
		texColor = getPaletteColor64(texColor);		//使用64位调色板，可表达26万色
		gl_FragColor = texColor;
	}
)";

const char* cc_palette_frag_noclamp = R"(
	#ifdef GL_ES
	precision highp float;
  	#endif

	varying vec4 v_fragmentColor;
	varying vec2 v_texCoord;

	//uniform sampler2D CC_Texture0;
	//uniform sampler2D CC_Texture1;

	// palette: 256*16
	vec4 getPaletteColor16(vec4 texColor)
	{
		vec4 paletteColor = texture2D(CC_Texture1, vec2(texColor.r / 16.0 + floor(texColor.b * 16.0) / 16.0, texColor.g));
		paletteColor *= texColor.a;
		return paletteColor;
	}

	// palette: 1024*32
	vec4 getPaletteColor32(vec4 texColor)
	{
		vec4 paletteColor = texture2D(CC_Texture1, vec2(texColor.r / 32.0 + floor(texColor.b * 32.0) / 32.0, texColor.g));
		paletteColor *= texColor.a;
		return paletteColor;
	}

	// palette: 512*512(4096*64)
	vec4 getPaletteColor64(vec4 texColor)
	{
		float index = floor(texColor.b * 64.0);
		float u = texColor.r / 8.0 + mod(index, 8.0) / 8.0;
		float v = texColor.g / 8.0 + floor(index / 8.0) / 8.0;
		vec4 paletteColor = texture2D(CC_Texture1, vec2(u, v));
		paletteColor *= texColor.a;
		return paletteColor;
	}

	void main()
	{
		vec4 texColor = texture2D(CC_Texture0, v_texCoord);// * v_fragmentColor;
		texColor = getPaletteColor64(texColor);		//使用64位调色板，可表达26万色
		gl_FragColor = texColor;
	}
)";

PaletteTest::PaletteTest()
{
	ADD_TEST_CASE(PaletteTest64);
	ADD_TEST_CASE(PaletteTest64_2p);
	ADD_TEST_CASE(PaletteTest64NoClamp);
	ADD_TEST_CASE(PaletteCreate);
}

PaletteCreate::PaletteCreate()
{
}

std::string PaletteCreate::title() const
{
	return "Palette Test";
}

std::string PaletteCreate::subtitle() const
{
	return "Create Palette";
}

bool PaletteCreate::init()
{
	if (!TestCase::init())
		return false;

	// Save Image menu
	MenuItemFont::setFontSize(16);
	auto item1 = MenuItemFont::create("Save Palette 16", CC_CALLBACK_0(PaletteCreate::savePalette, this, 16));
	auto item2 = MenuItemFont::create("Save Palette 32", CC_CALLBACK_0(PaletteCreate::savePalette, this, 32));
	auto item3 = MenuItemFont::create("Save Palette 64", CC_CALLBACK_0(PaletteCreate::savePalette, this, 64));
	auto item4 = MenuItemFont::create("Save Palette -1", CC_CALLBACK_0(PaletteCreate::savePalette, this, -1));
	auto menu = Menu::create(item1, item2, item3, item4, nullptr);
	this->addChild(menu);
	menu->alignItemsVertically();
	menu->setPosition(Vec2(VisibleRect::rightTop().x - 80, VisibleRect::rightTop().y - 100));

	return true;
}

void PaletteCreate::savePalette(int nSize)
{
	unsigned char* buffer = nullptr;
	int nWidth = 0, nHeight = 0;
	if (nSize == 16 || nSize == 32)
	{
		nWidth = nSize * nSize;
		nHeight = nSize;
		buffer = new unsigned char[nWidth * nHeight * 4];
		memset(buffer, 255, nWidth * nHeight * 4);

		int nBlockSize = 256 / nSize;

		for (int r = 0; r < nSize; ++r)
		{
			for (int g = 0; g < nSize; ++g)
			{
				for (int b = 0; b < nSize; ++b)
				{
					int row = g;
					int col = r + b * nSize;
					int pos = (row * nWidth + col) * 4;
					buffer[pos++] = r * nBlockSize;
					buffer[pos++] = g * nBlockSize;
					buffer[pos++] = b * nBlockSize;
				}
			}
		}
	}
	else if (nSize == 64)	//64*64=4096，长度过大，改成8*8*64
	{
		nWidth = nSize * 8;
		nHeight = nSize * 8;
		buffer = new unsigned char[nWidth * nHeight * 4];
		memset(buffer, 255, nWidth * nHeight * 4);

		int nBlockSize = 256 / nSize;

		for (int r = 0; r < nSize; ++r)
		{
			for (int g = 0; g < nSize; ++g)
			{
				for (int b = 0; b < nSize; ++b)
				{
					int row = g + b / 8 * nSize;
					int col = r + b % 8 * nSize;
					int pos = (row * nWidth + col) * 4;
					buffer[pos++] = r * nBlockSize;
					buffer[pos++] = g * nBlockSize;
					buffer[pos++] = b * nBlockSize;
				}
			}
		}
	}
	else if (nSize < 0)
	{
		nWidth = 1024;
		nHeight = 1024;
		buffer = new unsigned char[nWidth * nHeight * 4];
		memset(buffer, 255, nWidth * nHeight * 4);

		auto _drawColor = [=](int r, int g, int b, int cr = -1, int cg = -1, int cb = -1) {
			int row = g + b / 2;
			int col = r + b / 2;
			int pos = (row * nWidth + col) * 4;
			buffer[pos++] = cr >= 0 ? cr : r;
			buffer[pos++] = cg >= 0 ? cg : g;
			buffer[pos++] = cb >= 0 ? cb : b;
		};

		//画颜色块
		for (int r = 255; r >= 0; --r)
		{
			for (int g = 255; g >= 0; --g)
			{
				for (int b = 255; b >= 0; --b)
				{
					if (b % 15 == 0)
					{
						_drawColor(r, g, b);
					}
				}
			}
		}

		//画边线
		int nSkip = 0;
		for (int i = 0; i < 256; ++i)
		{
			_drawColor(i, 0, 0, 0, 0, 0);
			_drawColor(0, i, 0, 0, 0, 0);
			_drawColor(0, 0, i, 0, 0, 0);
			_drawColor(0, 255, i, 0, 0, 0);
			_drawColor(255, 0, i, 0, 0, 0);
			_drawColor(0, i, 255, 0, 0, 0);
			_drawColor(i, 0, 255, 0, 0, 0);
			_drawColor(255, i, 255, 0, 0, 0);
			_drawColor(i, 255, 255, 0, 0, 0);


			if (nSkip-- > 0)
			{
				continue;
			}

			if (i % 16 == 0)
			{
				nSkip = 4;
				continue;
			}

			_drawColor(255, i, 0);
			_drawColor(i, 255, 0);
			_drawColor(255, 255, i);
		}
	}

	if (!buffer || !nWidth * nHeight)
		return;

	Image *image = new (std::nothrow) Image();
	image->initWithRawData(buffer, nWidth * nHeight * 4, nWidth, nHeight, 8, false);

	char fileName[MAX_PATH] = {};
	sprintf(fileName, "%spalette_%d.png", FileUtils::getInstance()->getWritablePath().c_str(), nSize);
	image->saveToFile(fileName);

	CC_SAFE_DELETE(image);
	CC_SAFE_DELETE_ARRAY(buffer);
}

Sprite* _createPaletteSprite(const std::string fileName, const std::string paletteName,
							const std::string vertSource, const std::string fragSource)
{
	auto img = Sprite::create(fileName);
	if (img)
	{
		auto r = img->getTextureRect();
		auto s = Director::getInstance()->getWinSize();
		img->setPosition(Vec2((s.width - r.size.width) / 2, (s.height - r.size.height) / 2));

		auto program = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
		auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
		img->setGLProgramState(glProgramState);

		Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(paletteName);
		if(texture)
			img->getTexture()->setAlphaTexture(texture);
	}

	return img;
}

PaletteTest64::PaletteTest64()
{
}

std::string PaletteTest64::title() const
{
	return "Palette Test";
}

std::string PaletteTest64::subtitle() const
{
	return "Palette 64 bit";
}


bool PaletteTest64::init()
{
	if (!TestCase::init())
		return false;

	Sprite* img = _createPaletteSprite("Palette/test_palette.png", "", ccPositionTexture_vert, ccPositionTexture_frag);
	this->addChild(img);
	return true;
}

PaletteTest64_2p::PaletteTest64_2p()
{
}

std::string PaletteTest64_2p::title() const
{
	return "Palette Test";
}

std::string PaletteTest64_2p::subtitle() const
{
	return "Palette 64 bit 2p";
}


bool PaletteTest64_2p::init()
{
	if (!TestCase::init())
		return false;

	Sprite* img = _createPaletteSprite("Palette/test_palette.png", "Palette/palette_2p.png", cc_palette_vert, cc_palette_frag);
	this->addChild(img);
	return true;
}

PaletteTest64NoClamp::PaletteTest64NoClamp()
{
}

std::string PaletteTest64NoClamp::title() const
{
	return "Palette Test";
}

std::string PaletteTest64NoClamp::subtitle() const
{
	return "Palette 64 bit with no clamp";
}


bool PaletteTest64NoClamp::init()
{
	if (!TestCase::init())
		return false;

	Sprite* img = _createPaletteSprite("Palette/test_palette.png", "Palette/palette_2p.png", cc_palette_vert, cc_palette_frag_noclamp);
	this->addChild(img);
	return true;
}
