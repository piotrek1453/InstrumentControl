#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <visa.h>
#include <visatype.h>

constexpr ViAccessMode INSTRUMENT_ACCESS_MODE{VI_NULL};
constexpr ViUInt32 INSTRUMENT_TIMEOUT_MS{200};
constexpr size_t INSTRUMENT_BUFFER_SIZE_B{65536};

class VISAResource : public ResourceIfc
{
public:
  ~VISAResource() override = default;

  static auto create(LoggerIfc &logger,
                     std::string resourceString,
                     ViSession resourceManagerHandle)
      -> std::unique_ptr<VISAResource>;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  explicit VISAResource(LoggerIfc &logger,
                        std::string resourceString,
                        ViSession resourceManagerHandle) noexcept;

  LoggerIfc &logger_;
  std::string mResourceString;
  ViSession mResourceManagerHandle, mInstrumentSessionHandle;
  std::atomic<bool> mIsOpen{false};
  ViUInt32 mIOBytes{};
};
