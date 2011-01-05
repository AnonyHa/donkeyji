int main()
{
	event_init();

	conn_init();

	server_init();

	event_dispatch();

	return 0;
}
