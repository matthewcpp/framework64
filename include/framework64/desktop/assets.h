#pragma once

#include "framework64/assets.h"

#include <string>

class fw64Assets {
public:
    fw64Assets(std::string const & base_path);
    bool init();

    fw64Texture* getTexture(int handle);

private:
    std::string asset_dir;
};

