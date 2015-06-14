int main(int argc, char* args[]) {
#ifdef TEST
	extern int testMain(int argc, char* args[]);
	return testMain(argc, args);
#else
	extern int appMain(int argc, char* args[]);
	return appMain(argc, args);
#endif
}