#pragma once
# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include  "PrimeSet.h"
#include "GameInfo.h"
//全てのSceneからアクセスできるデータ
struct GameData
{
	//セーブできるものは↓↓
	Array<bool>optionActive;//オプションがON/OFF切り替え可能かどうか
	PrimeSet primeSet;//素数コレクション
	Array<bool>trophy;//実績一覧


	//セーブデータでないものは↓↓
	struct
	{
		bool noMiss;
		bool noItems;
		bool freeFall;
		bool hardCore;
		bool fiftySeven;
		bool hexaDecimal;
	}options;//オプションがONかOFFか
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