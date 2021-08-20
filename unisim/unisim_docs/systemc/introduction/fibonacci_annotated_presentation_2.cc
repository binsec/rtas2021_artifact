void write(unsigned int address, unsigned int val) {
	/* SystemC code to send a write request to the memory */
	...
}

unsigned int read(unsigned int address) {
	/* SystemC code to send a read request to the memory */
	...
	return read_value;
}
