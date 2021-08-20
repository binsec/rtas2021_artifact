class MyClient : public Client<ServiceInterface> //(1)
{
public:
	ServiceImport<ServiceInterface> my_interface_import; //(2)

	MyClient(const char *name, Object *parent = 0) : //(3)
		Object(name, parent), //(4)
		Client<ServiceInterface>(name, parent), //(5)
		my_interface_import("my-interface-import", this) //(6)
	{
	}
};