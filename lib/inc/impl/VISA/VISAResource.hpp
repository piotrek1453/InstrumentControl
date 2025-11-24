#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <array>
#include <atomic>
#include <string_view>
#include <visa.h>
#include <visatype.h>

constexpr ViAccessMode INSTRUMENT_ACCESS_MODE{VI_NULL};
constexpr ViUInt32 INSTRUMENT_TIMEOUT_MS{200};
constexpr size_t INSTRUMENT_BUFFER_SIZE_B{32000000};

class VISAResource : public ResourceIfc
{
public:
  explicit VISAResource(LoggerIfc &logger,
                        std::string_view resourceString,
                        ViSession resourceManagerHandle) noexcept;
  ~VISAResource() override = default;

  auto write(std::string_view command) -> bool override;
  auto read() -> ReadResult override;
  auto query(std::string_view command) -> ReadResult override;

private:
  LoggerIfc &logger_;
  std::string_view mResourceString;
  ViSession mResourceManagerHandle, mInstrumentSessionHandle;
  std::array<ViChar, INSTRUMENT_BUFFER_SIZE_B> mBuffer{0};
  std::atomic<bool> mIsOpen{false};
};
