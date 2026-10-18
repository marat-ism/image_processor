// ./filters input.bmp output.bmp -crop 800 600 -blur 0.5

#include <stdexcept>
#include <string_view>
#include <vector>
#include <utility>
#include <iostream>

#include "errors.h"
#include "factory.h"
#include "controller.h"

struct FilterSpec {
    std::string_view name;
    std::vector<std::string_view> params;
};

struct Args {
    std::string_view input;
    std::string_view output;
    std::vector<FilterSpec> filters;
};

Args ParseArgs(const std::vector<std::string_view>& params) {
    Args answer;
    if (params.size() < 2u) {
        throw UsageError("not enough params");
    }
    answer.input = params[0];
    answer.output = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
        FilterSpec filter;
        filter.name = params[i].substr(1);
        if (filter.name.starts_with('-')) {
            throw UnknownFilterError(std::string(filter.name));
        }
        auto j = i + 1;
        while (j < params.size() && !params[j].starts_with('-')) {
            filter.params.push_back(params[j]);
            ++j;
        }
        i = j - 1;
        answer.filters.push_back(filter);
    }
    return answer;
}

int main(int argc, char** argv) {
    const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
    try {
        const Args args = ParseArgs(raw_args);
        std::vector<std::unique_ptr<ip::Filter>> filters;
        for (const auto& filter : args.filters) {
            filters.push_back(ip::factory::CreateFilter(filter.name, filter.params));
        }
        ip::ImageProcessor processor(std::move(filters));
        processor.Run(args.input, args.output);
        return 0;
    } catch (const UnknownFilterError& e) {
        std::cerr << "error: " << e.what() << '\n';
        for (const std::string_view name : ip::factory::KnownFilters()) {
            std::cerr << " -" << name;
        }
        std::cerr << '\n';
        return 1;
    } catch (const UsageError& e) {
        std::cerr << "usage error: " << e.what() << '\n';
        return 2;
    } catch (const ImageProcessorError& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 3;
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 10;
    } catch (...) {
        std::cerr << "unknown error\n";
        return 100;
    }
    
}

