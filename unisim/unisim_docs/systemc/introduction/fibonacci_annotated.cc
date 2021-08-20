unsigned int cur_address = 0;

void write(unsigned int address, unsigned int val) {
	/* SystemC code to send a write request to the memory */
	...
}

unsigned int read(unsigned int address) {
	/* SystemC code to send a read request to the memory */
	...
	return read_value;
}

void fibonacci() {
	unsigned int n = read(cur_address);
	if (1 == n || 2 == n) {
		write(cur_address, 1);
	} else {
		cur_address++;
		write(cur_address, n - 1);
		fibonacci();
		cur_address++;
		write(cur_address, n - 2);
		fibonacci();
		cur_address -= 2;
		write(cur_address, 
				read(cur_address + 1) + read(cur_address + 2));
	}
}

int main() {
	unsigned int result;
	
	write(cur_address, 10);
	fibonnaci();
	result = read(cur_address);
	
	return 0;
}

