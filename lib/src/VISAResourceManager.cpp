#include "impl/VISAResourceManager.hpp"
#include <memory>

VISAResourceManager::VISAResourceManager(std::shared_ptr<LoggerIfc> logger)
    : logger_(logger) {
  logger->info("VISAResourceManager created");
}
