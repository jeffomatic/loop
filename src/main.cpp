extern int appMain(int argc, char* args[]);
extern int testMain(int argc, char* args[]);

int main(int argc, char* args[]) {
#ifdef TEST
	return testMain(argc, args);
#else
	return appMain(argc, args);
#endif
}