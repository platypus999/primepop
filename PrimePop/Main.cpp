//# include <Siv3D.hpp>
#include "Title.h"
#include "DiffMenu.h"
#include "Game.h"
#include "Result.h"
#include "Rule.h"
#include "Achievement.h"

void Main()
{
	Window::Resize(405, 720);
	Window::SetTitle(GameInfo::Title + L" " + GameInfo::Version);
	Graphics::SetBackground(GameInfo::BackgroundColor);
	System::SetExitEvent(WindowEvent::CloseButton);

	MyApp manager;

	manager.add<Title>(L"Title");
	manager.add<DiffMenu>(L"DiffMenu");
	manager.add<Game>(L"Game");
	manager.add<Result>(L"Result");
	manager.add<Rule>(L"Rule");
	manager.add<Achievement>(L"Achievement");

	FontAsset::Register(L"Title", GameInfo::TitleFontSize, Typeface::Heavy, FontStyle::Outline);
	FontAsset::Register(L"Menu", GameInfo::MenuFontSize, Typeface::Bold);
	FontAsset::Register(L"TitleFooter", 10, Typeface::Light, FontStyle::Italic);
	FontAsset::Register(L"Lever", GameInfo::LeverFontSize);
	FontAsset::Register(L"Switch", GameInfo::SwitchFontSize);
	FontAsset::Register(L"Descript", 11, L"ＭＳゴシック");
	FontAsset::Register(L"Countdown", GameInfo::TitleFontSize, Typeface::Heavy);
	FontAsset::Register(L"Score", GameInfo::ScoreFontSize, L"ＭＳ Ｐゴシック");
	FontAsset::Register(L"ScoreLetter", GameInfo::ScoreLetterFontSize);
	FontAsset::Register(L"Bubble", GameInfo::BubbleFontSize, Typeface::Heavy);
	FontAsset::Register(L"Equation", GameInfo::EquationFontSize);
	FontAsset::Register(L"Button", GameInfo::ButtonFontSize);
	FontAsset::Register(L"DescriptSmall", GameInfo::DescriptSmallFontSize);
	FontAsset::Register(L"Trophy", 15);

	while (System::Update())
	{
		if (!manager.updateAndDraw())
		{
			break;
		}
	}
}