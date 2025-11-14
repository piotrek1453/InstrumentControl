#include "impl/VISAResource.hpp"
#include <memory>

VISAResource::VISAResource(std::shared_ptr<LoggerIfc> logger)
    : logger_(logger) {
  logger->info("VISAResource created");
}
