namespace tlm {

// Initiator socket
template <
	unsigned int BUSWIDTH = 32,
	typename FW_IF = tlm_fw_nb_transport_if<>,
	typename BW_IF = tlm_bw_nb_transport_if<> >
class tlm_initiator_socket : public sc_core::sc_port<FW_IF> {
public:
	typedef FW_IF fw_interface_type;
	typedef BW_IF bw_interface_type;
	typedef sc_core::sc_port<fw_interface_type> port_type;
	typedef sc_core::sc_export<bw_interface_type> export_type;
	typedef tlm_ target_socket<BUSWIDTH, 
			fw_interface_type, 
			bw_interface_type> target_socket_type;
	tlm_initiator_socket( const char* );
	unsigned int get_bus_width() const;
	void bind( target_socket_type& );
	void operator() ( target_socket_type& );
	void bind( tlm_initiator_socket& );
	void operator() ( tlm_initiator_socket& );
	void bind( bw_interface_type& );
	void operator() ( bw_interface_type& );
protected:
	export_type mExport;
};

} // namespace tlm
