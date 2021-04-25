#pragma once
#include "GameData.h"

void GameSave(std::shared_ptr<GameData> g)
{
	Serializer<BinaryWriter> writer(GameInfo::SaveFilePath);
	writer(g->optionActive);
	writer(g->primeSet);
	writer(g->trophy);
	writer.getWriter().close();
}

void GameLoad(std::shared_ptr<GameData> g)
{
	if (!FileSystem::Exists(GameInfo::SaveFilePath))
	{
		return;
	}
	Deserializer<BinaryReader> reader(GameInfo::SaveFilePath);
	reader(g->optionActive);
	reader(g->primeSet);
	reader(g->trophy);
}