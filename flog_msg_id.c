//! Flog - Message ID definitions for the F logging library

//! @file flog_msg_id.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Useful for saving space on embedded systems, performance etc.
//! if FLOG_NO_MSG_ID_STRINGS is defined then the static string
//! data will not be included. This can be used for deeply embedded
//! systems where string generation isn't strictly necessary,
//! and can be decoded by the receiver.

#include "flog_msg_id.h"

#ifdef FLOG_CONFIG_STRING_OUTPUT
#ifdef FLOG_CONFIG_MSG_ID_STRINGS
#define X(id, str) str,
const char *flog_msg_id_str[] = {
	FLOG_MSG_IDS
	0
};
#undef X
#endif //FLOG_CONFIG_MSG_ID_STRINGS
#endif //FLOG_CONFIG_STRING_OUTPUT
