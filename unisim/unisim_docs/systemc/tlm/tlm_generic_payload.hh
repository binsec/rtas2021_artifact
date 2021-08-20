namespace tlm {

class tlm_generic_payload {
public:
	// Constructors, assignment, destructor
	tlm_generic_payload();
	tlm_generic_payload( const tlm_generic_payload& );
	tlm_generic_payload& operator= ( const tlm_generic_payload& );
	tlm_generic_payload* deep_copy() const;
	virtual ~tlm_generic_payload();

	// Access methods
	inline tlm_command get_command() const;
	inline void set_command( const tlm_command );
	inline bool is_read();
	inline void set_read();
	inline bool is_write();
	inline void set_write();
	inline sc_dt::uint64 get_address() const;
	inline void set_address( const sc_dt::uint64 );
	inline unsigned char* get_data_ptr() const;
	inline void set_data_ptr( unsigned char* );
	inline unsigned int get_data_length() const;
	inline void set_data_length( const unsigned int );
	inline unsigned int get_streaming_width() const;
	inline void set_streaming_width( const unsigned int );
	inline bool* get_byte_enable_ptr() const;
	inline void set_byte_enable_ptr( bool* );
	inline unsigned int get_byte_enable_length() const;
	inline void set_byte_enable_length( const unsigned int );
	
	// DMI hint
	void set_dmi_allowed( bool );
	bool get_dmi_allowed() const;
	
	// response status
	inline tlm_response_status get_response_status() const;
	inline void set_response_status( const tlm_response_status );
	inline std::string get_response_string();
	inline bool is_response_ok();
	inline bool is_response_error();
	
	// Extension mechanism
	template <typename T> T* set_extension( T* );
	tlm_extension_base* set_extension( unsigned int , tlm_extension_base* );
	template <typename T> void get_extension( T*& ) const;
	tlm_extension_base* get_extension( unsigned int ) const;
	template <typename T> void clear_extension( const T* );
	void clear_extension( unsigned int );
	void resize_extensions();
};

} // namespace tlm
