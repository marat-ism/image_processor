#pragma once

#include <stdexcept>
#include <string>

class ImageProcessorError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class UsageError : public ImageProcessorError {
public:
    using ImageProcessorError::ImageProcessorError;
};

class UnknownFilterError : public ImageProcessorError {
public:
    explicit UnknownFilterError(const std::string& type)
        : ImageProcessorError("unknown filter type: " + type), type_(type) {}
    const std::string& Type() const { return type_; }

private:
    std::string type_;
};

class InvalidParameterError : public ImageProcessorError {
public:
    InvalidParameterError() : ImageProcessorError("filter parameter is invalid") {}
};

class BMPError : public ImageProcessorError {
public:
    using ImageProcessorError::ImageProcessorError;
};