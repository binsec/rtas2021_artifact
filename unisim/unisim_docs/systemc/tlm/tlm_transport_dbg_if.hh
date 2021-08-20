namespace tlm {

template <typename TRANS = tlm_generic_payload>
class tlm_transport_dbg_if : public virtual sc_core::sc_interface {
public:
	virtual unsigned int transport_dbg(TRANS& trans) = 0;
};

} // namespace tlm
