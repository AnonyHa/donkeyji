//pack response，发送出去
void done_callbak()
{}

int main()
{
	event_init();

	server_init();

	event_dispatch();

	return 0;
}
