namespace tlm {

class tlm_extension_base {
public:
	virtual tlm_extension_base* clone() const = 0;
	virtual ~tlm_extension_base() {}
};

template <typename T>
class tlm_extension : 
	public tlm_extension_base {
public:
	virtual tlm_extension_base* clone() const = 0;
	virtual ~tlm_extension() {}
	const static unsigned int ID;
};

} // namespace tlm
