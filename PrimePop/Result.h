#pragma once
#include <Siv3D.hpp>
#include "GameData.h" 
#include "Button.h"
#include "GameSave.h"

//���ʉ��
class Result : public MyApp::Scene
{
private:
	String plotString;
	Array<Button>buttons;
	bool isCleared;
public:
	~Result()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init() override
	{
		//�`�悷�镶����
		plotString = GameInfo::ModeSet[m_data->endless] + L"�@" + GameInfo::DifficulityLevel[m_data->difficulity];
		//�{�^���̔z�u
		const int rad = 50;
		Circle menu((double)Window::Width() * (1. / 6), (double)Window::Height() * (double)(5. / 6), rad);
		Circle retr((double)Window::Width() * (3. / 6), (double)Window::Height() * (double)(5. / 6), rad);
		Circle twee((double)Window::Width() * (5. / 6), (double)Window::Height() * (double)(5. / 6), rad);
		//100�b���[�h�ŃQ�[���I�[�o�[�̎�
		if (!m_data->endless && m_data->lastResult.time != GameInfo::timeLimit)
		{
			menu.setPos((double)Window::Width() * (1. / 4), (double)Window::Height() * (3. / 4));
			retr.setPos((double)Window::Width() * (3. / 4), (double)Window::Height() * (3. / 4));
			buttons.push_back(Button(menu, L"���j���[", Palette::Green));
			buttons.push_back(Button(retr, L"�������", Palette::Red));
			isCleared = false;
		}
		else
		{
			buttons.push_back(Button(menu, L"���j���[", Palette::Green));
			buttons.push_back(Button(retr, L"�������", Palette::Red));
			buttons.push_back(Button(twee, L"�c�C�[�g", Palette::Blue));
			isCleared = !m_data->endless;
		}
		if (isCleared && !m_data->endless)
		{
			//���щ����@100�b�N���A
			if (!m_data->trophy[m_data->difficulity])
			{
				m_data->trophy[m_data->difficulity] = true;
				if (m_data->difficulity == 3)
				{
					//�n�[�h�R�A
					m_data->optionActive[3] = true;
				}
			}
		}
		else
		{
			//���щ����E����
			int id = 4 * (m_data->difficulity + 1);
			if (m_data->lastResult.time >= 200s)
			{
				if (!m_data->trophy[id + 0])
				{
					m_data->trophy[id + 0] = true;
				}
			}
			if (m_data->lastResult.time >= 300s)
			{
				if (!m_data->trophy[id + 1])
				{
					m_data->trophy[id + 1] = true;
				}
			}
			if (m_data->lastResult.time >= 500s)
			{
				if (!m_data->trophy[id + 2])
				{
					m_data->trophy[id + 2] = true;
					if (m_data->difficulity > 0)
					{
						//���R����
						m_data->optionActive[2] = true;
					}
				}
			}
			if (m_data->lastResult.time >= 1000s)
			{
				if (!m_data->trophy[id + 3])
				{
					m_data->trophy[id + 3] = true;
				}
			}
		}
		//���щ����E���_
		if (m_data->lastResult.score >= 1000)
		{
			if (!m_data->trophy[20 + 0])
			{
				m_data->trophy[20 + 0] = true;
			}
		}
		if (m_data->lastResult.score >= 5000)
		{
			if (!m_data->trophy[20 + 1])
			{
				m_data->trophy[20 + 1] = true;
			}
		}
		if (m_data->lastResult.score >= 10000)
		{
			if (!m_data->trophy[20 + 2])
			{
				m_data->trophy[20 + 2] = true;
			}
		}
		if (m_data->lastResult.score >= 20000)
		{
			if (!m_data->trophy[20 + 3])
			{
				m_data->trophy[20 + 3] = true;
			}
		}
		if (m_data->lastResult.score % 100 == 16)
		{
			//16�i��
			m_data->optionActive[5] = true;
		}
		//���щ����E�f��
		m_data->primeSet.checkPrime(m_data->trophy);
		//���щ����E���̑�
		if (m_data->lastResult.time >= 100s && m_data->options.freeFall)
		{
			if (!m_data->trophy[24])
			{
				m_data->trophy[24] = true;
			}
		}
		if (m_data->lastResult.time >= 100s && m_data->options.hardCore && m_data->difficulity == 3)
		{
			if (!m_data->trophy[25])
			{
				m_data->trophy[25] = true;
			}
		}
		if (m_data->lastResult.time >= 100s && m_data->options.hexaDecimal && m_data->difficulity > 0)
		{
			if (!m_data->trophy[26])
			{
				m_data->trophy[26] = true;
			}
		}
		//fill(m_data->optionActive.begin(), m_data->optionActive.end(), true);
		GameSave(m_data);
	}
	void update() override
	{
		bool mouse = false;
		if (buttons[0].update(mouse))
		{
			//���j���[�ɖ߂�
			changeScene(L"Title");
			return;
		}
		if (buttons[1].update(mouse))
		{
			//������x���
			changeScene(L"Game");
			return;
		}
		if (buttons.size() > 2 && buttons[2].update(mouse))
		{
			//�c�C�[�g����
			String tweetMessage = Format(GameInfo::Title, L" ", GameInfo::Version, L"���v���C���܂����B\n", plotString, L" �� ", m_data->lastResult.score, L"�_�����܂����B");
			
			Array<String>options;
			Array<bool>optionList = { m_data->options.noMiss,m_data->options.noItems,m_data->options.freeFall,m_data->options.hardCore,m_data->options.fiftySeven,m_data->options.hexaDecimal };
			for (auto i : step(GameInfo::SwitchString.size()))
			{
				if (optionList[i])
				{
					options.push_back(GameInfo::SwitchString[i]);
				}
			}
			if (!options.empty())
			{
				tweetMessage += L"(";
				for (auto i : step(options.size()))
				{
					tweetMessage += options[i];
					if (i != options.size() - 1)tweetMessage += L" ";
				}
				tweetMessage += L")";
			}
			if (m_data->endless)
			{
				tweetMessage += Format(L"(�������ԁF", DurationCast<Seconds>(m_data->lastResult.time), L")");
			}
			tweetMessage += L" #PrimePop";
			Twitter::OpenTweetWindow(tweetMessage);
		}
		//�}�E�X�m�F
		Cursor::SetStyle(mouse ? CursorStyle::Hand : CursorStyle::Default);
	}
	void draw() const override
	{
		FontAsset(L"Title")(L"���ʔ��\").drawCenter({ Window::Width() / 2, 50 });
		double s = (double)(Window::Width() - 40) / FontAsset(L"Menu")(plotString).region().w;
		Graphics2D::SetTransform(Mat3x2::Scale(s, { Window::Width() / 2,150 }));
		FontAsset(L"Menu")(plotString).drawCenter({ Window::Width() / 2,150 });
		Graphics2D::SetTransform(Mat3x2::Identity());
		FontAsset(L"Menu")(Format(m_data->lastResult.equation)).drawCenter({ Window::Width() / 2,200 }, isCleared ? Palette::Green : Palette::Red);
		FontAsset(L"Menu")(Format(L"Score:{0:06d}"_fmt, m_data->lastResult.score)).drawCenter({ Window::Width() / 2,400 });
		FontAsset(L"Menu")(Format(L"(", DurationCast<Seconds>(m_data->lastResult.time)), L")").drawCenter({ Window::Width() / 2,450 });
		for (auto&b : buttons)
		{
			b.draw();
		}
	}
};