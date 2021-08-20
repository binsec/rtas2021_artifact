namespace tlm {

// The forward non-blocking interface:
template< typename TYPES = tlm_generic_payload_types >
class tlm_fw_nb_transport_if : 
	public virtual tlm_nonblocking_transport_if<
		typename TYPES::tlm_payload_type ,
		typename TYPES::tlm_phase_type >,
	public virtual tlm_fw_direct_mem_if<
		typename TYPES::tlm_dmi_mode_type >,
		public virtual tlm_transport_dbg_if {};

// The backward non-blocking interface:
template < typename TYPES = tlm_generic_payload_types >
class tlm_bw_nb_transport_if : 
	public virtual tlm_nonblocking_transport_if<
		typename TYPES::tlm_payload_type ,
		typename TYPES::tlm_phase_type >,
	public virtual tlm_bw_direct_mem_if {};
	
}

