namespace tlm {

// Target socket
template <
	unsigned int BUSWIDTH = 32,
	typename FW_IF = tlm_fw_nb_transport_if<>,
	typename BW_IF = tlm_bw_nb_transport_if<> >
class tlm_target_socket : public sc_core::sc_export<FW_IF> {
public:
	typedef FW_IF fw_interface_type;
	typedef BW_IF bw_interface_type;
	typedef sc_core::sc_port<bw_interface_type> port_type;
	typedef sc_core::sc_export<fw_interface_type> export_type;
	typedef tlm_initiator_socket<BUSWIDTH, 
			fw_interface_type, 
			bw_interface_type> initiator_socket_type;
	tlm_target_socket( const char* );
	unsigned int get_bus_width() const;
	void bind( initiator_socket_type& );
	void operator() ( initiator_socket_type& );
	void bind( tlm_target_socket& );
	void operator() ( tlm_target_socket& );
	void bind( fw_interface_type& );
	void operator() ( fw_interface_type& );
	bw_interface_type* operator-> ();
protected:
	port_type mPort;
};

} // namespace tlm
