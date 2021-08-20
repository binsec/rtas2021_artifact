class MyService : public Service<MyInterface> //(1)
{
public:
	ServiceExport<MyInterface> my_interface_export; //(2)

	MyService(const char *name, Object *parent = 0) : //(3)
		Object(name, parent), //(4)
		Service<MyInterface>(name, parent), //(5)
		my_interface_export("my-interface-export", this) //(6)
	{
	}
};