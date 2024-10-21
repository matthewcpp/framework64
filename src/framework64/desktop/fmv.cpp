#include "fmv.hpp"

fw64FmvStatus fw64_fmv_get_status(fw64Fmv* fmv) {
    return fmv->status;
}

int fw64_fmv_open(fw64Fmv* fmv, fw64AssetId) {
    fmv->status = FW64_FMV_STOPPED;
    return 1;
}
void fw64_fmv_close(fw64Fmv* fmv) {
    fmv->status = FW64_FMV_CLOSED;
}

void fw64_fmv_play(fw64Fmv* fmv) {
    fmv->status = FW64_FMV_PLAYING;
}

void fw64_fmv_pause(fw64Fmv* fmv) {
    fmv->status = FW64_FMV_PAUSED;
}