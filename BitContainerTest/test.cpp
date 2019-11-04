#include "pch.h"
#include "../BitContainer.hpp"

class Small : public BitContainerValue<2, Small>
{
public:
	constexpr Small(Base b) : Base(b) {}
	constexpr static auto v0() { return createValue(0); }
	constexpr static auto v1() { return createValue(1); }
};

class Middle : public BitContainerValue<15, Middle>
{
public:
	constexpr Middle(Base b) : Base(b) {}
	constexpr static auto v0() { return createValue(0); }
	constexpr static auto v1() { return createValue(1); }
	constexpr static auto v2() { return createValue(2); }
	constexpr static auto v3() { return createValue(3); }
	constexpr static auto v4() { return createValue(4); }
	constexpr static auto v5() { return createValue(5); }
	constexpr static auto v6() { return createValue(6); }
	constexpr static auto v14() { return createValue(14); }
};

class Big : public BitContainerValue<64, Big>
{
public:
	constexpr Big(Base b) : Base(b) {}
	constexpr static auto v0() { return createValue(0); }
	constexpr static auto v15() { return createValue(15); }
	constexpr static auto v16() { return createValue(16); }
	constexpr static auto v32() { return createValue(32); }
	constexpr static auto v63() { return createValue(63); }
};

using SmallC = BitContainer<Small>;
using MidC = BitContainer<Middle>;
using BigC = BitContainer<Big>;

class BitContainerTest : public ::testing::Test {
public:
	BitContainerTest() : smallC(Small::v0(), Small::v1()) {}
	SmallC smallC;
	MidC midC = MidC(Middle::v0(), Middle::v1(), Middle::v2(), Middle::v3(), Middle::v4(), Middle::v5(), Middle::v6(), Middle::v14());
	BigC bigC = BigC(Big::v0(), Big::v15(), Big::v16(), Big::v32(), Big::v63());
};

TEST_F(BitContainerTest, size)
{
	EXPECT_EQ(smallC.size(), 2);
	EXPECT_EQ(midC.size(), 8);
	EXPECT_EQ(bigC.size(), 5);
}

TEST_F(BitContainerTest, capacity)
{
	EXPECT_EQ(smallC.capacity(), 2);
	EXPECT_EQ(midC.capacity(), 15);
	EXPECT_EQ(bigC.capacity(), 64);
}

TEST_F(BitContainerTest, contains)
{
	EXPECT_EQ(smallC.contains(Small::v0()), true);
	EXPECT_EQ(midC.contains(Middle::v14()), true);
	EXPECT_EQ(bigC.contains(Big::v63()), true);
}

TEST_F(BitContainerTest, clear)
{
	smallC.clear();
	midC.clear();
	bigC.clear();

	EXPECT_EQ(smallC.contains(Small::v1()), false);
	EXPECT_EQ(midC.contains(Middle::v5()), false);
	EXPECT_EQ(bigC.contains(Big::v32()), false);

	EXPECT_EQ(smallC.size(), 0);
	EXPECT_EQ(midC.size(), 0);
	EXPECT_EQ(bigC.size(), 0);

	EXPECT_EQ(smallC.empty(), true);
	EXPECT_EQ(midC.empty(), true);
	EXPECT_EQ(bigC.empty(), true);
}

TEST_F(BitContainerTest, erase)
{
	EXPECT_EQ(smallC.contains(Small::v1()), true);
	EXPECT_EQ(midC.contains(Middle::v5()), true);
	EXPECT_EQ(bigC.contains(Big::v32()), true);

	smallC.erase(Small::v1());
	midC.erase(Middle::v5());
	bigC.erase(Big::v32());

	EXPECT_EQ(smallC.contains(Small::v1()), false);
	EXPECT_EQ(midC.contains(Middle::v5()), false);
	EXPECT_EQ(bigC.contains(Big::v32()), false);
}

TEST_F(BitContainerTest, insert)
{
	smallC.clear();
	midC.clear();
	bigC.clear();

	smallC.insert(Small::v1());
	midC.insert(Middle::v5());
	bigC.insert(Big::v32());

	EXPECT_EQ(smallC.size(), 1);
	EXPECT_EQ(midC.size(), 1);
	EXPECT_EQ(bigC.size(), 1);

	EXPECT_EQ(smallC.contains(Small::v1()), true);
	EXPECT_EQ(midC.contains(Middle::v5()), true);
	EXPECT_EQ(bigC.contains(Big::v32()), true);
}

TEST_F(BitContainerTest, correctIterationSteps)
{
	int cnt = 0;
	for (auto v : bigC)
		++cnt;
	EXPECT_EQ(cnt, bigC.size());

	cnt = 0;
	for (auto v : smallC)
		++cnt;
	EXPECT_EQ(cnt, smallC.size());

	cnt = 0;
	for (auto v : midC)
		++cnt;
	EXPECT_EQ(cnt, midC.size());
}

TEST_F(BitContainerTest, correctIterationDerefValues)
{
	int cnt = 0;
	for (auto v : smallC)
	{
		if (cnt == 0)
			EXPECT_EQ(v, Small::v0());
		if (cnt == 1)
			EXPECT_EQ(v, Small::v1());
		++cnt;
	}

	cnt = 0;
	for (auto v : midC)
	{
		if (cnt == 0)
			EXPECT_EQ(v, Middle::v0());
		if (cnt == 14)
			EXPECT_EQ(v, Middle::v14());
		++cnt;
	}

	cnt = 0;
	for (auto v : bigC)
	{
		if (cnt == 15)
			EXPECT_EQ(v, Big::v15());
		if (cnt == 32)
			EXPECT_EQ(v, Big::v32());
		if (cnt == 63)
			EXPECT_EQ(v, Big::v63());
		++cnt;
	}
}

TEST_F(BitContainerTest, copyConstructor)
{
	auto newSmallC = smallC;
	EXPECT_EQ(newSmallC.contains(Small::v0()), true);
	EXPECT_EQ(newSmallC.contains(Small::v1()), true);
}

TEST_F(BitContainerTest, comparison)
{
	auto newSmallC = smallC;
	EXPECT_EQ(newSmallC == smallC, true);
	newSmallC.erase(Small::v0());
	EXPECT_EQ(newSmallC != smallC, true);
}