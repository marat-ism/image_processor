#pragma once

#include <vector>
#include <memory>

#include "filter.h"
#include "bmp.h"

namespace ip {

class ImageProcessor {
public:
    explicit ImageProcessor(std::vector<std::unique_ptr<Filter>> filters);
    void Run(std::string_view input_path, std::string_view output_path) const;

private:
    std::vector<std::unique_ptr<Filter>> filters_;
};

}