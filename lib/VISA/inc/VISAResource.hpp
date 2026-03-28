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
constexpr size_t INSTRUMENT_BUFFER_SIZE_B{32768};

class VISAResource : public ResourceIfc
{
public:
  /**
   * Closes the opened VISA instrument session.
   */
  ~VISAResource();

  /**
   * Factory that validates opening the instrument and returns nullptr on
   * failure.
   */
  static auto create(LoggerIfc &logger,
                     std::string resourceString,
                     ViSession resourceManagerHandle)
      -> std::unique_ptr<VISAResource>;

  /**
   * Sends a command to the instrument.
   */
  auto write(const std::string &command) -> bool override;

  /**
   * Reads a response from the instrument.
   */
  auto read() -> ReadResult override;

  /**
   * Sends a command and reads the response.
   */
  auto query(const std::string &command) -> ReadResult override;

private:
  explicit VISAResource(LoggerIfc &logger,
                        std::string resourceString,
                        ViSession resourceManagerHandle) noexcept;

  LoggerIfc &logger_;
  const std::string mResourceString;
  ViSession mResourceManagerHandle, mInstrumentSessionHandle;
  std::atomic<bool> mIsOpen{false};
  ViUInt32 mIOBytes{};
};
