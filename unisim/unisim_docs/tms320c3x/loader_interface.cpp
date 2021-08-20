template <class T>
class Loader
{
public:
	virtual void Reset() = 0;
	virtual T GetEntryPoint() const = 0;
	virtual T GetTopAddr() const = 0;
	virtual T GetStackBase() const = 0;
};
