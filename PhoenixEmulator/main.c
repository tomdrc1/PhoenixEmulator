#include <stdio.h>
#include <stdlib.h>
#include "phoenixArcadeMachine.h"


int main(int argc, char** argv)
{
	phoenixArcadeMachine* machine = (phoenixArcadeMachine*)malloc(sizeof(phoenixArcadeMachine));

	startEmulation(machine);
	return 0;
}