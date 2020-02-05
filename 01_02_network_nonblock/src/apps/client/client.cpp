
#include <list>
#include <memory>
#include "client_socket.h"

int main(int argc, char *argv[])
{
	std::list<std::shared_ptr<ClientSocket>> clients;
	for (int index = 0; index < 3; index++)
	{
		clients.push_back(std::make_shared<ClientSocket>(index));
	}

	while (!clients.empty())
	{
		auto iter = clients.begin();
		while (iter != clients.end())
		{
			auto pClient = (*iter);
			if ((*pClient).IsRun())
			{
				++iter;
				continue;
			}

			pClient->Stop();
			iter = clients.erase(iter);
		}
	}

	return 0;
}

