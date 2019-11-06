// C. Röhl - EddyXorb 2019

#pragma once
#include <array>
#include <vector>

template <class First, class... Rest>
constexpr bool are_same()
{
   std::array<bool, sizeof...(Rest)> couples = {std::is_same<First, Rest>::value...};
   for (const auto& isSameCouple : couples)
      if (!isSameCouple)
         return false;
   return true;
};

template <class BitContainerValueT>
class BitContainer;

// Derived classes should implement the same constructor and move it into
// protected or private. Furthermore implement meaningful functions like "static
// constexpr auto NAME()-> Derived {return createValue(position); }" in derived
// class.
template <std::size_t N, class Derived>
class BitContainerValue
{
   static_assert((N <= 64), "Cannot create BitContainerValue with more than 64 entries!");

public:
   template <class BitContainerValue_T>
   friend class BitContainer;

   constexpr friend bool operator==(const Derived& l, const Derived& r) { return l.value == r.value; }
   constexpr friend bool operator!=(const Derived& l, const Derived& r) { return !(l == r); }
   constexpr friend bool operator<(const Derived& l, const Derived& r) { return l.value < r.value; }

protected:
   using underlying_type = std::conditional_t<(N < 17), unsigned short, std::conditional_t<(N < 33), unsigned int, unsigned long long>>;
   using Base = BitContainerValue<N, Derived>;
   constexpr static auto createValue(std::size_t position) -> Derived;
   static constexpr auto size() -> std::size_t { return N; }

private:
   underlying_type value;

   constexpr BitContainerValue(std::size_t position) : value(static_cast<underlying_type>(1) << position) {}
};

template <class BitContainerValueT>
class BitContainer
{
public:
   using value_type = BitContainerValueT;
   class const_iterator;

   constexpr void insert(value_type val);

   template <class... Args>
   constexpr auto insert(value_type first, Args... args) -> std::enable_if_t<are_same<value_type, Args...>(), void>;

   template <class... Args>
   constexpr BitContainer(Args... args)
   {
      insert(args...);
   }

   constexpr auto erase(value_type val) -> const_iterator;
   constexpr void clear() { this->value = 0; }
   constexpr auto size() const -> std::size_t;

   constexpr bool contains(value_type val) { return (this->value & val.value) == val.value; };
   constexpr bool empty() const;
   constexpr auto capacity() const -> std::size_t { return value_type::size(); }

   constexpr auto begin() const -> const_iterator;
   constexpr auto end() const -> const_iterator;

   constexpr bool operator==(BitContainer r) { return this->value == r.value; }
   constexpr bool operator!=(BitContainer r) { return this->value != r.value; }
   constexpr bool operator<(BitContainer r) { return this->value < r.value; }

   class const_iterator
   {
      friend class BitContainer<BitContainerValueT>;

   public:
      constexpr auto operator*() const -> value_type;
      constexpr auto operator++() -> const_iterator;
      constexpr auto operator++(int) -> const_iterator;
      constexpr auto operator--() -> const_iterator;
      constexpr auto operator--(int) -> const_iterator;

      constexpr bool operator==(const_iterator other) const { return it == other.it && &ref == &other.ref; }
      constexpr bool operator!=(const_iterator other) const { return !(*this == other); }

   private:
      constexpr static auto endIt(const BitContainer& ref) { return const_iterator(static_cast<unsigned short>(value_type::size()), ref); }

      constexpr const_iterator(std::size_t pos, const BitContainer& ref) : it(static_cast<unsigned short>(pos)), ref(ref) {}

      constexpr auto isEnd() const { return it == (static_cast<unsigned short>(value_type::size())); }

      const BitContainer& ref;
      unsigned short it;
   };

private:
   constexpr void insert() {}
   typename value_type::underlying_type value = 0;
   static constexpr auto getPos(value_type val) -> std::size_t;
   static constexpr auto createValue(std::size_t pos) { return value_type(typename value_type::Base(pos)); }

   constexpr bool test(std::size_t pos) const { return (this->value & (static_cast<decltype(value)>(1) << pos)) != 0; }
};

template <std::size_t N, class Derived>
inline constexpr auto BitContainerValue<N, Derived>::createValue(std::size_t position) -> Derived
{
   if (position > BitContainerValue::size() - 1)
      throw std::runtime_error("Tried to create BitContainerValue that is bigger than maximale size!");
   return Derived(BitContainerValue(position));
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::const_iterator::operator*() const -> value_type
{
   if (this->isEnd())
      throw std::runtime_error("Tried to dereference end-iterator!");
   return BitContainer::createValue(it);
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::const_iterator::operator++() -> const_iterator
{
   if (this->isEnd())
      throw std::runtime_error("BitContainer iterator out of range");

   ++it;
   while (!(isEnd()) && !this->ref.test(it))
      ++it;

   return *this;
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::const_iterator::operator++(int) -> const_iterator
{
   auto out = *this;
   ++(*this);
   return out;
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::const_iterator::operator--() -> const_iterator
{
   --it;
   while (it > 0 && !this->ref.test(it))
   {
      --it;
   }
   if (!this->ref.test(it))
      throw std::runtime_error("BitContainer iterator out of range");

   return *this;
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::const_iterator::operator--(int) -> const_iterator
{
   auto out = *this;
   --*this;
   return out;
}

template <class BitContainerValueT>
inline constexpr void BitContainer<BitContainerValueT>::insert(value_type val)
{
   this->value = this->value | val.value;
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::erase(value_type val) -> const_iterator
{
   this->value = this->value & (std::numeric_limits<decltype(value)>::max() - val.value);

   if (this->empty())
      return end();

   auto pos = getPos(val);
   ++pos;
   while (pos < value_type::size() && !test(pos))
      ++pos;

   return const_iterator(pos, *this);
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::size() const -> std::size_t
{
   std::size_t out = 0;

   for (std::size_t i = 0; i < value_type::size(); ++i)
      if ((this->value & (static_cast<decltype(value)>(1) << i)) != 0)
         ++out;

   return out;
}

template <class BitContainerValueT>
inline constexpr bool BitContainer<BitContainerValueT>::empty() const
{
   constexpr auto allOne = std::numeric_limits<decltype(value)>::max();
   return (this->value & allOne) == 0;
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::begin() const -> const_iterator
{
   if (this->empty())
      return const_iterator::endIt(*this);

   int pos = 0;
   while ((this->value & (static_cast<decltype(value)>(1) << pos)) == 0)
      ++pos;

   return const_iterator(pos, *this);
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::end() const -> const_iterator
{
   return const_iterator::endIt(*this);
}

template <class BitContainerValueT>
inline constexpr auto BitContainer<BitContainerValueT>::getPos(value_type val) -> std::size_t
{
   for (std::size_t i = 0; i < val.size(); ++i)
      if ((val.value & (static_cast<decltype(value)>(1) << i)) != 0)
         return i;

   throw std::runtime_error(
       "Given BitContainerValue does not contain any set "
       "bit. Check derived class.");
   return 0;
}

template <class BitContainerValueT>
template <class... Args>
inline constexpr auto BitContainer<BitContainerValueT>::insert(value_type first, Args... args)
    -> std::enable_if_t<are_same<value_type, Args...>(), void>
{
   insert(first);
   insert(args...);
}
