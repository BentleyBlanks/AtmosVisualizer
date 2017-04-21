#pragma once
#include "a3MessageQueue.h"

enum A3_C2S_MESSAGES
{
    A3_C2S_MSG_GRIDIMAGE = 0,
    // --!in the future
    A3_C2S_MSG_LIGHTPATH
};

enum A3_S2C_MESSAGES
{
    A3_S2C_MSG_INIT = 0
};

#define A3_MESSAGE_DEFINE(MessageClass, MessageType)      \
	MessageClass()  { type = MessageType; }            \

// Server to Client
struct a3S2CInitMessage : public a3MessageEntryHead
{
    int fuck;
    A3_MESSAGE_DEFINE(a3S2CInitMessage, A3_S2C_MSG_INIT)
};

// Client to Server
struct a3C2SGridImageMessage : public a3MessageEntryHead
{
    int shit;
    A3_MESSAGE_DEFINE(a3C2SGridImageMessage, A3_C2S_MSG_GRIDIMAGE)
};