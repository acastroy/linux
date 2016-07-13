#ifndef MST3367_H
#define MST3367_H

#include "LINUXV4L2.h"

BOOLEAN MST3367_HwInitialize( CDevice * pDevice );
BOOLEAN MST3367_VideoFormatDetection( CDevice * pDevice );

#endif //MST3367_H