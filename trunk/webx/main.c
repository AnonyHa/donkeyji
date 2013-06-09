int main()
{
	config_init();
	signal_init();
	timer_init();

	server_init();

	server_dispatch();

	return 0;
}
