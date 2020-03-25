
#include "console.h"

class ConsoleCmdPool :public ConsoleCmd
{
public:
	void RegisterHandler() override;

private:
	void HandleHelp(std::vector<std::string>& params);
	void HandleShow(std::vector<std::string>& params);

};
