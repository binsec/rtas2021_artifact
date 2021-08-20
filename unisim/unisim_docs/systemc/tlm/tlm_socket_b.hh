namespace tlm {

// Convenience sockets using blocking transport interfaces
template < unsigned int BUSWIDTH = 32, 
		 typename TYPES = tlm_generic_payload_types >
class tlm_b_initiator_socket : 
	public tlm_initiator_socket< BUSWIDTH,
		tlm_fw_b_transport_if<TYPES>,
		tlm_bw_b_transport_if<TYPES> > {
public:
	tlm_b_initiator_socket(const char* name) : 
		tlm_initiator_socket(name) {}
}

template < unsigned int BUSWIDTH = 32, 
		 typename TYPES = tlm_generic_payload_types >
class tlm_b_target_socket :
	public tlm_target_socket< BUSWIDTH,
		tlm_fw_b_transport_if<TYPES>,
		tlm_bw_b_transport_if<TYPES> > {
public:
	tlm_b_target_socket(const char* name) : 
		tlm_target_socket(name) {}
}

} // namespace tlm
