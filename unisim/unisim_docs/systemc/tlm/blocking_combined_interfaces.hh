namespace tlm {

// The forward blocking interface:
template < typename TYPES = tlm_generic_payload_types >
class tlm_fw_b_transport_if : 
	public virtual tlm_blocking_transport_if<
		typename TYPES::tlm_payload_type >,
	public virtual tlm_fw_direct_mem_if<
		typename TYPES::tlm_dmi_mode_type>,
	public virtual tlm_transport_dbg_if {};

// The backward blocking interface:
class tlm_bw_b_transport_if : 
	public virtual tlm_bw_direct_mem_if {};

}

