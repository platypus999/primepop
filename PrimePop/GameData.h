#pragma once
# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include  "PrimeSet.h"
#include "GameInfo.h"
//�S�Ă�Scene����A�N�Z�X�ł���f�[�^
struct GameData
{
	//�Z�[�u�ł�����̂́���
	Array<bool>optionActive;//�I�v�V������ON/OFF�؂�ւ��\���ǂ���
	PrimeSet primeSet;//�f���R���N�V����
	Array<bool>trophy;//���шꗗ


	//�Z�[�u�f�[�^�łȂ����̂́���
	struct
	{
		bool noMiss;
		bool noItems;
		bool freeFall;
		bool hardCore;
		bool fiftySeven;
		bool hexaDecimal;
	}options;//�I�v�V������ON��OFF��
	int difficulity;//0~3
	bool endless;
	struct
	{
		int score;
		Milliseconds time;
		String equation;
	}lastResult;

	GameData()
	{
		trophy.resize(GameInfo::TrophyString.size());
		optionActive.resize(GameInfo::SwitchString.size());
		optionActive[0] = optionActive[1] = true;
	}
};

using MyApp = SceneManager<String, GameData>;