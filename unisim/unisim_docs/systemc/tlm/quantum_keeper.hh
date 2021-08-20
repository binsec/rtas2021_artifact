namespace tlm {

class tlm_quantumkeeper {
public:
	static void set_global_quantum( const sc_core::sc_time& );
	static const sc_core::sc_time& get_global_quantum();

	tlm_quantumkeeper();
	virtual ~tlm_quantumkeeper();

	void inc( const sc_core::sc_time& );
	sc_core::sc_time get_current_time() const;
	const sc_core::sc_time& get_local_time() const;
	sc_core::sc_time& get_local_time();
	bool need_sync() const;
	void reset();
	void sync();

protected:
	virtual sc_core::sc_time compute_local_quantum();
};

} // namespace tlm
