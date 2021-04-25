#pragma once
#include <Siv3D.hpp>
#include <set>

class PrimeSet
{
private:
	std::set<int>prime;
	bool isAllOK(Array<int>arr)
	{
		for (int s : arr)
		{
			if (!prime.count(s))
			{
				return false;
			}
		}
		return true;
	}
	
public:
	template <class Archive>
	void save(Archive& archive)const
	{
		Array<int> arr;
		for (int s : prime)arr.push_back(s);
		archive(arr);
	}
	template <class Archive>
	void load(Archive& archive)
	{
		Array<int> arr;
		archive(arr);
		for (int s : arr)prime.insert(s);
	}
	void insertPrime(int P)
	{
		prime.insert(P);
	}
	Array<int> getAllPrimes()
	{
		Array<int>res;
		for (int p : prime)
		{
			res.push_back(p);
		}
		return res;
	}
	void checkPrime(Array<bool> &trophy)
	{
		//é¿ê—ÇÃâèúÇÇ∑ÇÈ
		//27,28.29
		if (isAllOK({ 3,7,31,127 }) && !trophy[27])
		{
			trophy[27] = true;
		}
		if (isAllOK({ 3,5,17,257 }) && !trophy[28])
		{
			trophy[28] = true;
		}
		if (isAllOK({ 41, 43, 47, 53, 61, 71, 83, 97, 113, 131, 151, 173, 197, 223, 251, 281, 313, 347, 383, 421, 461, 503, 547, 593, 641, 691, 743, 797, 853, 911, 971 })
			&& !trophy[29])
		{
			trophy[29] = true;
		}
		int twocnt = 0,cnt = 0;
		for (int p : prime)
		{
			if (p <= 100)twocnt++;
			if (p <= 1000)cnt++;
		}
		//32
		if (twocnt == 25 && !trophy[32])
		{
			trophy[32] = true;
		}
		//33,34,35,36,37,38
		if ((double)cnt >= 168 * 0.5 && !trophy[33])
		{
			trophy[33] = true;
		}
		if ((double)cnt >= 168 * 0.75 && !trophy[34])
		{
			trophy[34] = true;
		}
		if ((double)cnt >= 168 * 0.90 && !trophy[35])
		{
			trophy[35] = true;
		}
		if ((double)cnt >= 168 * 0.95 && !trophy[36])
		{
			trophy[36] = true;
		}
		if ((double)cnt >= 168 * 0.99 && !trophy[37])
		{
			trophy[37] = true;
		}
		if ((double)cnt >= 168 * 1 && !trophy[38])
		{
			trophy[38] = true;
		}
	}
	PrimeSet() = default;
	PrimeSet(const PrimeSet& other) = default;
};