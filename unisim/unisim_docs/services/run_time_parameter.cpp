class MyService : public Service<MyInterface>
{
public:
	Parameter<unsigned int> param_x; //(2)

	MyService(const char *name, Object *parent = 0) :
		Object(name, parent),
		Service<MyInterface>(name, parent),
		param_x("x", this, x) //(3)
	{
	}
private:
	unsigned int x; //(1)
};