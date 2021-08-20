template <class T>
class Symbol {
public:
	enum Type {
		SYM_NOTYPE = 0,
		SYM_OBJECT = 1,
		SYM_FUNC = 2,
		SYM_SECTION = 3,
		SYM_FILE = 4,
		SYM_COMMON = 5,
		SYM_TLS = 6,
		SYM_NUM = 7,
		SYM_LOOS = 8,
		SYM_HIOS = 9,
		SYM_LOPROC = 10,
		SYM_HIPROC = 11
	};

	Symbol(const char *name, T addr, T size, typename unisim::util::debug::Symbol<T>::Type type, T memory_atom_size);
	const char *GetName() const;
	T GetAddress() const;
	T GetSize() const;
	typename unisim::util::debug::Symbol<T>::Type GetType() const;
	string GetFriendlyName(T addr) const;
};

template <class T>
class SymbolTableLookup {
public:
	virtual const typename unisim::util::debug::Symbol<T> *FindSymbol(
		const char *name,
		T addr,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;

	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByAddr(T addr) const = 0;
	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByName(const char *name) const = 0;
	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByName(
		const char *name,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;
	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByAddr(
		T addr,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;
};
