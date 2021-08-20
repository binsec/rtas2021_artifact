namespace tlm {

// The forward interface:
template <typename TYPES = tlm_base_protocol_types>
class tlm_fw_transport_if
  : public virtual tlm_fw_nonblocking_transport_if<typename TYPES::tlm_payload_type, typename TYPES::tlm_phase_type>
  , public virtual tlm_blocking_transport_if<typename TYPES::tlm_payload_type>
  , public virtual tlm_fw_direct_mem_if<typename TYPES::tlm_payload_type>
  , public virtual tlm_transport_dbg_if<typename TYPES::tlm_payload_type>
{};

// The backward interface:
template <typename TYPES = tlm_base_protocol_types>
class tlm_bw_transport_if
  : public virtual tlm_bw_nonblocking_transport_if<typename TYPES::tlm_payload_type, typename TYPES::tlm_phase_type>
  , public virtual tlm_bw_direct_mem_if
{};
	
}
