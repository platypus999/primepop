#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "GameInfo.h"
#include "MenuEffect.h"
#include "GameSave.h"

//タイトル画面
class Title : public MyApp::Scene
{
private:
	Effect effect;
	Stopwatch menuItemStopwatch{ true };
	Array<Rect>menuBoxes;
	Array<String>menuTexts =
	{
		GameInfo::MenuGameStart,
		GameInfo::MenuRules,
		GameInfo::MenuScore,
		GameInfo::MenuExit,
	};
public:
	~Title()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init()override
	{
		GameLoad(m_data);
		menuBoxes.resize(menuTexts.size());
		//一番長い文字に幅を合わせる
		int32 boxWidth = 0;
		for (const auto& text : menuTexts)
		{
			boxWidth = Max(boxWidth, FontAsset(L"Menu")(text).region().w);
		}
		for (auto i : step(menuBoxes.size()))
		{
			menuBoxes[i] = Rect(boxWidth + 80, 60).setCenter(Window::Center().x, 280 + i * 80);
		}
	}
	void update() override
	{
		bool handCursor = false;
		if (Input::KeyControl.pressed && Input::KeyDelete.clicked)
		{
			if (MessageBox::Show(L"セーブデータを削除します！\n本当によろしいですか？", MessageBoxStyle::YesNo) == MessageBoxCommand::Yes)
			{
				FileSystem::Remove(GameInfo::SaveFilePath);
				System::Exit();
				return;
			}
		}
		for (auto i : step(menuBoxes.size()))
		{
			const auto& item = menuBoxes[i];
			handCursor |= item.mouseOver;
			if (item.mouseOver && menuItemStopwatch.elapsed() > 300ms)
			{
				effect.add<MenuEffect>(menuBoxes[i]);
				menuItemStopwatch.restart();
			}
			if (item.leftClicked)
			{
				if (i == 0)
				{
					changeScene(L"DiffMenu");
				}
				else if (i == 1)
				{
					changeScene(L"Rule");
				}
				else if (i == 2)
				{
					changeScene(L"Achievement");
				}
				else
				{
					System::Exit();
				}
				break;
			}
		}
		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);
	}
	void draw() const override
	{
		Graphics2D::SetBlendState(BlendState::Additive);
		effect.update();
		Graphics2D::SetBlendState(BlendState::Default);
		const double titleHeight = FontAsset(L"Title")(GameInfo::Title).region().h;
		FontAsset(L"Title")(GameInfo::Title).drawAt(Window::Center().x, titleHeight);
		for (auto i : step(menuBoxes.size()))
		{
			menuBoxes[i].draw();
			FontAsset(L"Menu")(menuTexts[i]).drawAt(menuBoxes[i].center, Color(40));
		}
		FontAsset(L"TitleFooter")(GameInfo::TitleFooter).drawAt(Window::Center().x, Window::Height() - 40);
	}
};