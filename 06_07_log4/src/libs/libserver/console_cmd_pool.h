
#include "console.h"

class ConsoleCmdPool :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

private:
	void HandleShow(std::vector<std::string>& params);
};
