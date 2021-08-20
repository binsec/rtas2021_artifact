unsigned int fibonacci(unsigned int n) {
    if (1 == n || 2 == n) {
        return 1;
    } else {
        return (fibonacci(n-1) + fibonacci(n-2));
    }
}

