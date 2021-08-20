class MemoryRequest {
public:
	enum MemoryRequestType {
		WRITE_COMMAND,
		READ_COMMAND
	};
	
	unsigned int target_id;
	unsigned int write_data;
	MemoryRequestType command;
};

class MemoryResponse {
public:
	unsigned int read_data;
};
