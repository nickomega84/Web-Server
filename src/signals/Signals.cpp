/* #include "../../include/signals/Signals.hpp"

Signals::Signals()
{}

~Signals::Signals()
{}

void Signals::handleSignals()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN); // Ignorar SIGPIPE para evitar que el servidor termine en caso de desconexión del cliente
}

 */