unsigned int cur_address = 0;

void fibonacci() {
	unsigned int n = read(cur_address);
	if (1 == n || 2 == n) {
		write(cur_address, 1);
	} else {
		unsigned int res1, res2;
		cur_address++;
		write(cur_address, n - 1);
		fibonacci();
		cur_address++;
		write(cur_address, n - 2);
		fibonacci();
		cur_address -= 2;
		write(cur_address, read(cur_address + 1), 
				read(cur_address + 2));
	}
}

int main() {
	unsigned int result;
	
	write(cur_address, 10);
	fibonnaci();
	result = read(cur_address);
	
	return 0;
}


