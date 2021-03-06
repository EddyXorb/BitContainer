// BitContainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "BitContainer.hpp"

class Prop : public BitContainerValue<7, Prop>
{
public:
	constexpr Prop(Base base) : Base(base) {}

	static constexpr auto isHigh() -> Prop { return Base::createValue(0); }
	static constexpr auto isLow() -> Prop { return Base::createValue(1); }
	static constexpr auto isTiny() -> Prop { return Base::createValue(2); }
	static constexpr auto isBig() -> Prop { return Base::createValue(4); }
	static constexpr auto isHuge() -> Prop { return Base::createValue(6); }
};

using Props = BitContainer<Prop>;

int main()
{
	Props c;
	c.insert(Prop::isHigh());
	c.insert(Prop::isHigh());
	c.insert(Prop::isLow());

	constexpr Props c2(Prop::isTiny(), Prop::isBig(), Prop::isHuge());

	std::cout << "C1: " << c.size() << "\n";
	std::cout << "C1: " << c.capacity() << "\n";
	std::cout << "C1: " << c.empty() << "\n";

	c.erase(Prop::isLow());

	std::cout << "C1: " << c.size() << "\n";
	std::cout << "C1: " << c.capacity() << "\n";
	std::cout << "C1: " << c.empty() << "\n";

	c.erase(Prop::isHigh());

	std::cout << "C1: " << c.size() << "\n";
	std::cout << "C1: " << c.capacity() << "\n";
	std::cout << "C1: " << c.empty() << "\n";

	std::cout << "C2: " << c2.size() << "\n";
	std::cout << "C2: " << c2.capacity() << "\n";
	std::cout << "C2: " << c2.empty() << "\n";

	constexpr auto sizeOfC2 = c2.size();
	constexpr auto beginOfC2 = c2.begin();
	constexpr auto endOfC2 = c2.end();

	auto cBegin = c2.begin();

	c = Props(Prop::isTiny(), Prop::isLow());

	for (auto it = c.begin(); it != c.end(); ++it)
	{
		if (*it == Prop::isLow())
			std::cout << "isLow!" << "\n";
	}

	for (auto prop : c)
	{
		if (prop == Prop::isLow())
			std::cout << "isLow!" << "\n";
	}
}

