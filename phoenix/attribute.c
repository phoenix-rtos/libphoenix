/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * phoenix/attribute
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <phoenix/attribute.h>


void _phoenix_initAttrsStruct(struct _attrAll *attrs, int err)
{
	attrs->mode.err = err;
	attrs->uid.err = err;
	attrs->gid.err = err;
	attrs->size.err = err;
	attrs->blocks.err = err;
	attrs->blocks.err = err;
	attrs->ioblock.err = err;
	attrs->type.err = err;
	attrs->port.err = err;
	attrs->pollStatus.err = err;
	attrs->eventMask.err = err;
	attrs->cTime.err = err;
	attrs->mTime.err = err;
	attrs->aTime.err = err;
	attrs->links.err = err;
	attrs->dev.err = err;
}
