namespace tlm {

class tlm_dmi {
public:
	enum dmi_access_e
	{ DMI_ACCESS_NONE = 0x00                    // no access
	, DMI_ACCESS_READ = 0x01                    // read access
	, DMI_ACCESS_WRITE = 0x02                   // write access
	, DMI_ACCESS_READ_WRITE = DMI_ACCESS_READ |
	                          DMI_ACCESS_WRITE  // read/write access
	};
	
	tlm_dmi();
	void init();

	unsigned char *get_dmi_ptr() const;
	sc_dt::uint64 get_start_address() const;
	sc_dt::uint64 get_end_address() const;
	sc_core::sc_time get_read_latency() const;
	sc_core::sc_time get_write_latency() const;
	dmi_access_e get_granted_access() const;
	bool is_none_allowed() const;
	bool is_read_allowed() const;
	bool is_write_allowed() const;
	bool is_read_write_allowed() const;

	void set_dmi_ptr(unsigned char *p);
	void set_start_address(sc_dt::uint64 addr);
	void set_end_address(sc_dt::uint64 addr);
	void set_read_latency(sc_core::sc_time t);
	void set_write_latency(sc_core::sc_time t);
	void set_granted_access(dmi_access_e a);
	void allow_none();
	void allow_read();
	void allow_write();
	void allow_read_write();
};

template <typename TRANS = tlm_generic_payload>
class tlm_fw_direct_mem_if : public virtual sc_core::sc_interface {
public:
	virtual bool get_direct_mem_ptr(TRANS& trans, tlm_dmi& dmi_data) = 0;
};

class tlm_bw_direct_mem_if : public virtual sc_core::sc_interface {
public:
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) = 0;
};

} // namespace tlm
