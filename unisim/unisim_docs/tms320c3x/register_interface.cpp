class Register
{
public:
	virtual ~Register() {}
	virtual const char *GetName() const = 0;
	virtual void GetValue(void *buffer) const = 0;
	virtual void SetValue(const void *buffer) = 0;
	virtual int GetSize() const = 0;
};
