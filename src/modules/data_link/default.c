#include "framework64/data_link.h"

struct fw64DataLink {
    int is_active;
};


static fw64DataLink default_data_link;

fw64DataLink* fw64_data_link_init(void* arg) {
    return &default_data_link;
}

void fw64_data_link_update(fw64DataLink* data_link) {

}

void fw64_data_link_set_connected_callback(fw64DataLink* data_link, fw64DataLinkConnectedCallback callback, void* arg) {

}

void fw64_data_link_set_mesage_callback(fw64DataLink* data_link, fw64DataLinkMessageCallback callback, void* arg) {

}

int fw64_data_link_connected(fw64DataLink* data_link) {
    return 0;
}

void fw64_data_link_send_message(fw64DataLink* data_link, fw64DataLinkMessageType message_type, const void* data, int data_size) {

}
