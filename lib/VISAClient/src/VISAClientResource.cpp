#include "../inc/VISAClientResource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include "pyvisa_grpc.pb.h"
#include <fmt/core.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <memory>
#include <string>

VISAClientResource::~VISAClientResource()
{
  if (mIsOpen)
  {
    pyvisa_grpc::DisconnectRequest req;
    grpc::ClientContext ctx;

    req.set_resource_name(mResourceString);

    pyvisa_grpc::StatusResponse resp;
    grpc::Status status = mStub->Disconnect(&ctx, req, &resp);

    if (!status.ok())
    {
      mLogger.log(
          fmt::format("Error disconnecting instrument\nResource string: "
                      "{}\nError: {} - {}",
                      mResourceString,
                      status.error_message(),
                      static_cast<int>(status.error_code())),
          LogLevel::Error);
    }
    else if (!resp.success())
    {
      mLogger.log(
          fmt::format("Error disconnecting instrument\nResource string: "
                      "{}\nError: {}",
                      mResourceString,
                      resp.message()),
          LogLevel::Error);
    }
    else
    {
      mLogger.log(fmt::format(
          "Disconnected VISAClientResource with resource string {}",
          mResourceString));
    }
  }
}

auto VISAClientResource::create(
    LoggerIfc &logger,
    const std::string &resource_string,
    std::shared_ptr<grpc::Channel> channel)
    -> std::unique_ptr<VISAClientResource>
{
  auto resourcePointer = std::unique_ptr<VISAClientResource>(
      new VISAClientResource(logger, resource_string, channel));

  if (!resourcePointer->mIsOpen)
  {
    return nullptr;
  }

  return resourcePointer;
}

VISAClientResource::VISAClientResource(
    LoggerIfc &logger,
    const std::string &resource_string,
    std::shared_ptr<grpc::Channel> channel)
    : mLogger(logger),
      mResourceString(resource_string),
      mChannel_ptr(channel),
      mStub(pyvisa_grpc::PyVISAService::NewStub(mChannel_ptr))
{
  pyvisa_grpc::ConnectRequest req;
  grpc::ClientContext ctx;

  req.set_resource_name(mResourceString);

  pyvisa_grpc::StatusResponse resp;
  grpc::Status status = mStub->Connect(&ctx, req, &resp);

  if (!status.ok())
  {
    mLogger.log(fmt::format("Error connecting to instrument\nResource string: "
                            "{}\nError: {} - {}",
                            mResourceString,
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Error);
    mIsOpen = false;
  }
  else if (!resp.success())
  {
    mLogger.log(fmt::format("Error connecting to instrument\nResource string: "
                            "{}\nError: {}",
                            mResourceString,
                            resp.message()),
                LogLevel::Error);
    mIsOpen = false;
  }
  else
  {
    mLogger.log(
        fmt::format("Created VISAClientResource with resource string {}",
                    resource_string));
    mIsOpen = true;
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
    mLogger.log(
        fmt::format(
            "Error writing to instrument\nResource string: {}\nError: {} - {}",
            mResourceString,
            status.error_message(),
            static_cast<int>(status.error_code())),
        LogLevel::Warn);
    return false;
  }

  if (!resp.success())
  {
    mLogger.log(
        fmt::format(
            "Error writing to instrument\nResource string: {}\nError: {}",
            mResourceString,
            resp.message()),
        LogLevel::Warn);
    return false;
  }

  mLogger.log(fmt::format("Sent message: \"{}\"", command));
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
    mLogger.log(fmt::format("Error reading from instrument\nResource string: "
                            "{}\nError: {} - {}",
                            mResourceString,
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Warn);
    return ReadResult::failure();
  }

  if (!resp.status().success())
  {
    mLogger.log(
        fmt::format(
            "Error reading from instrument\nResource string: {}\nError: {}",
            mResourceString,
            resp.status().message()),
        LogLevel::Warn);
    return ReadResult::failure();
  }

  mLogger.log(fmt::format("Received response: \"{}\"", resp.data()));
  return ReadResult::success(resp.data());
}

auto VISAClientResource::query(
    const std::string &command) -> ReadResult
{
  mLogger.log("VISAClientResource query", LogLevel::Debug);

  pyvisa_grpc::QueryRequest req;
  grpc::ClientContext ctx;

  req.set_resource_name(mResourceString);
  req.set_command(command);

  pyvisa_grpc::ReadResponse resp;
  grpc::Status status = mStub->Query(&ctx, req, &resp);

  if (!status.ok())
  {
    mLogger.log(fmt::format("Error querying instrument\nResource string: "
                            "{}\nError: {} - {}",
                            mResourceString,
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Warn);
    return ReadResult::failure();
  }

  if (!resp.status().success())
  {
    mLogger.log(
        fmt::format(
            "Error querying instrument\nResource string: {}\nError: {}",
            mResourceString,
            resp.status().message()),
        LogLevel::Warn);
    return ReadResult::failure();
  }

  mLogger.log(fmt::format("Received response: \"{}\"", resp.data()));
  return ReadResult::success(resp.data());
}
