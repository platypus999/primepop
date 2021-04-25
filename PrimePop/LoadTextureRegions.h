#pragma once
# include <Siv3D.hpp>

Array<TextureRegion> LoadTextureRegions(const FilePath& path, const Size& size)
{
	const Texture texture(path);

	Array<TextureRegion> textureRegions;

	for (auto p : step(Size(texture.width / size.x, texture.height / size.y)))
	{
		textureRegions.push_back(texture(p * size, size));
	}

	return textureRegions;
}