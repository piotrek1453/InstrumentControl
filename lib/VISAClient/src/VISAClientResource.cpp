#include "../inc/VISAClientResource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include "pyvisa_grpc.pb.h"
#include <fmt/core.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <memory>
#include <string>

VISAClientResource::VISAClientResource(
    LoggerIfc &logger,
    const std::string &resource_string,
    std::shared_ptr<grpc::Channel> channel)
    : mLogger(logger),
      mResourceString(resource_string),
      mChannel_ptr(channel),
      mStub(pyvisa_grpc::PyVISAService::NewStub(mChannel_ptr))
{
  mLogger.log(fmt::format("Created VISAClientResource with resource string {}",
                          resource_string));

  pyvisa_grpc::ConnectRequest req;
  grpc::ClientContext ctx;

  req.set_resource_name(mResourceString);

  pyvisa_grpc::StatusResponse resp;
  grpc::Status status = mStub->Connect(&ctx, req, &resp);

  // TODO: enhance logs here
  if (!status.ok())
  {
    mLogger.log("error connecting to instrument");
  }
}

auto VISAClientResource::write(
    const std::string &command) -> bool
{
  mLogger.log("VISAClientResource write", LogLevel::Debug);

  pyvisa_grpc::WriteRequest req;
  grpc::ClientContext ctx;

  req.set_resource_name(mResourceString);
  req.set_data(command);

  pyvisa_grpc::StatusResponse resp;
  grpc::Status status = mStub->Write(&ctx, req, &resp);

  if (!status.ok())
  {
    mLogger.log(fmt::format("gRPC error writing to instrument: {} (code={})",
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Warn);
    return false;
  }

  if (!resp.success())
  {
    mLogger.log(
        fmt::format("Instrument reported write failure: {}", resp.message()),
        LogLevel::Warn);
    return false;
  }

  mLogger.log("Sent message: \"" + command + '\"', LogLevel::Debug);
  return true;
}

auto VISAClientResource::read() -> ReadResult
{

  mLogger.log("VISAClientResource read", LogLevel::Debug);

  pyvisa_grpc::ResourceRequest req;
  grpc::ClientContext ctx;

  req.set_resource_name(mResourceString);

  pyvisa_grpc::ReadResponse resp;
  grpc::Status status = mStub->Read(&ctx, req, &resp);

  if (!status.ok())
  {
    mLogger.log(fmt::format("gRPC error reading from instrument: {} (code={})",
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Warn);
    return ReadResult::failure();
  }

  if (!resp.status().success())
  {
    mLogger.log(fmt::format("Instrument reported read failure: {}",
                            resp.status().message()),
                LogLevel::Warn);
    return ReadResult::failure();
  }

  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

auto VISAClientResource::query(
    const std::string &command) -> ReadResult
{
  mLogger.log("VISAClientResource query", LogLevel::Debug);
  // Simple placeholder: write then read
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
