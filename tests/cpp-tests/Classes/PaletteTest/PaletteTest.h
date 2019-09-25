#ifndef _MultiTexture_H__
#define _MultiTexture_H__

#include "cocos2d.h"
#include "../BaseTest.h"

using namespace cocos2d;

DEFINE_TEST_SUITE(PaletteTest);

// class PaletteTestScene : public TestCase
// {
// public:
// 	static PaletteTestScene* create();
// };

class PaletteCreate : public TestCase
{
public:
	CREATE_FUNC(PaletteCreate);
	PaletteCreate();

	virtual std::string title() const override;
	virtual std::string subtitle() const override;
	virtual bool init() override;

	void savePalette(int nSize);
};

class PaletteTest64 : public TestCase
{
public:
	CREATE_FUNC(PaletteTest64);
	PaletteTest64();

	virtual std::string title() const override;
	virtual std::string subtitle() const override;
	virtual bool init() override;
};

class PaletteTest64_2p : public TestCase
{
public:
	CREATE_FUNC(PaletteTest64_2p);
	PaletteTest64_2p();

	virtual std::string title() const override;
	virtual std::string subtitle() const override;
	virtual bool init() override;
};

class PaletteTest64NoClamp : public TestCase
{
public:
	CREATE_FUNC(PaletteTest64NoClamp);
	PaletteTest64NoClamp();

	virtual std::string title() const override;
	virtual std::string subtitle() const override;
	virtual bool init() override;
};

#endif // _MultiTexture_H__

