#include <stdio.h>
#include <stdlib.h>

#include "resetsettings.h"

void DoResetSettings()
{
    system("/bin/rsiutil clear");
}
