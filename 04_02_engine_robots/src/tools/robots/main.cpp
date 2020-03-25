
#include "libserver/server_app.h"
#include "robots_app.h"
#include "global_robots.h"

int main(int argc, char *argv[])
{
    GlobalRobots::Instance();
    return MainTemplate<RobotsApp>();
}
