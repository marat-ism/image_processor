#include "controller.h"



namespace ip {

ImageProcessor::ImageProcessor(std::vector<std::unique_ptr<Filter>> filters) : filters_(std::move(filters)) {}

void ImageProcessor::Run(std::string_view input_path, std::string_view output_path) const {
    Image image = bmp::Read(input_path);
    for (const auto& filter : filters_) {
        filter->Apply(image);
    }
    bmp::Write(image, output_path);
}

}