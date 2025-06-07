#pragma once

/** \file data_link.h */

#include <stdint.h>

#include "framework64/data_io.h"

typedef enum {
    FW64_DATA_LINK_MESSAGE_BINARY,
    Fw64_DATA_LINK_MESSAGE_TEXT
} fw64DataLinkMessageType;

typedef struct fw64DataLink fw64DataLink;

typedef void(*fw64DataLinkConnectedCallback)(void* arg);
typedef void(*fw64DataLinkMessageCallback)(fw64DataSource* message, void* arg);

#ifdef __cplusplus
extern "C" {
#endif

/** Private method used for initialization. Should not be used by callers. */
fw64DataLink* _fw64_data_link_init(void* arg);

/** Private method used for updating. Should not be used by callers. */
void _fw64_data_link_update(fw64DataLink* data_link);

void fw64_data_link_set_connected_callback(fw64DataLink* data_link, fw64DataLinkConnectedCallback callback, void* arg);
void fw64_data_link_set_mesage_callback(fw64DataLink* data_link, fw64DataLinkMessageCallback callback, void* arg);

int fw64_data_link_connected(fw64DataLink* data_link);
void fw64_data_link_send_message(fw64DataLink* data_link, fw64DataLinkMessageType message_type, const void* data, int data_size);

#ifdef __cplusplus
}
#endif
