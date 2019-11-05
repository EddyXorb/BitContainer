# BitContainer
## Save bool values in memory-optimal, constexpr and expressive way

A BitContainer is basically a Bit-Field (like std::bitset) with some abstraction to gain the usual "container-feeling" of the standard containers.
The main difference consists of how we access the bits. We don't do it directly, but use alias constexpr functions. Example:
```cpp
Props props;
props.insert(Prop::isHigh());
props.insert(Prop::isLow());

props.contains(Prop::isTiny()) // false
props.contains(Prop::isLow())  // true
```

The class **Prop** is user-defined and consists of static constexpr functions that basically return a wrapped integer of Type Prop. In that way we can take advantage of strong-typing to reduce code errors without sacrifying performance. 
The above snippet can also be evaluated at compiletime:

```cpp
constexpr Props props(Prop::isHigh(),Prop::isLow());

constexpr auto result1 = props.contains(Prop::isTiny()) // false
constexpr auto result2 = props.contains(Prop::isLow())  // true
```

How is the container **Props** defined?
```cpp
using Props = BitContainer<Prop>;
```
That easy. 
Do define the Value-representing class **Prop** we just have to type:
```cpp
class Prop : public BitContainerValue<3, Prop>
{
public:
	constexpr Prop(Base base) : Base(base) {}

	static constexpr auto isHigh() -> Prop { return Base::createValue(0); }
	static constexpr auto isLow() -> Prop { return Base::createValue(1); }
	static constexpr auto isTiny() -> Prop { return Base::createValue(2); }
};
```
Done!

We can also iterate over such a container, using even range-based for-loops:
```cpp
for (auto prop : props) // by-value since the underlying types are POD's
{	
    if (prop == Prop::isLow())
		std::cout << "isLow!";
}
```
There are all normally expected member functions of a standard container:
```cpp
props.insert(Prop::isHigh());
props.erase(Prop::isHigh()); 
props.size();              
props.capacity();         
props.empty();          
props.clear();
props.begin();
props.end();
```
For now you can save up to 64 values. This could be extended however if needed using std::array. If one day std::bitset gets full constexpr support, this will be even easier to extend.
