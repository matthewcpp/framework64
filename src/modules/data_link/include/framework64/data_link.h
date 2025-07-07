#pragma once

/** \file data_link.h */

#include <stdint.h>

#include <framework64/data_io.h>
#include <framework64/engine.h>

typedef enum {
    FW64_DATA_LINK_MESSAGE_BINARY,
    Fw64_DATA_LINK_MESSAGE_TEXT
} fw64DataLinkMessageType;

typedef struct fw64DataLink fw64DataLink;

#define FW64_DATALINK_MODULE_ID 1

typedef void(*fw64DataLinkConnectedCallback)(void* arg);
typedef void(*fw64DataLinkMessageCallback)(fw64DataSource* message, void* arg);

#ifdef __cplusplus
extern "C" {
#endif

/** This method should be called once during game initialization. */
fw64DataLink* fw64_data_link_init(fw64Engine* engine);

void fw64_data_link_set_connected_callback(fw64DataLink* data_link, fw64DataLinkConnectedCallback callback, void* arg);
void fw64_data_link_set_mesage_callback(fw64DataLink* data_link, fw64DataLinkMessageCallback callback, void* arg);

int fw64_data_link_connected(fw64DataLink* data_link);
void fw64_data_link_send_message(fw64DataLink* data_link, fw64DataLinkMessageType message_type, const void* data, int data_size);

#ifdef __cplusplus
}
#endif
