extern void donot(int *value);

void donot(int *value) {
	int i = *value;
	int j = ~i;
	*value = j;
}