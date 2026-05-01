#include "../inc/VISAClientResourceManager.hpp"
#include "../inc/VISAClientResource.hpp"
#include "pyvisa_grpc.grpc.pb.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/status.h>
#include <memory>
#include <string>
#include <vector>

VISAClientResourceManager::VISAClientResourceManager(
    LoggerIfc &logger,
    const std::string &serverAddress)
    : mLogger(logger)
{
  mChannel_ptr =
      grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
  mStub = pyvisa_grpc::PyVISAService::NewStub(mChannel_ptr);
}

auto VISAClientResourceManager::listAvailableResources() const
    -> std::vector<std::string>
{
  std::vector<std::string> results;
  pyvisa_grpc::ListResourcesRequest req;
  grpc::ClientContext ctx;

  auto reader = mStub->ListResources(&ctx, req);
  pyvisa_grpc::ListResourcesResponse resp;
  while (reader->Read(&resp))
  {
    // each streamed response contains a resource_name field and status
    if (!resp.status().success())
    {
      mLogger.log(fmt::format("ListResources item failure: {} (resource={})",
                              resp.status().message(),
                              resp.resource_name()),
                  LogLevel::Warn);
      continue;
    }

    results.push_back(resp.resource_name());
  }

  grpc::Status status = reader->Finish();
  if (!status.ok())
  {
    mLogger.log(fmt::format("Error discovering resources: {} (code={})",
                            status.error_message(),
                            static_cast<int>(status.error_code())),
                LogLevel::Warn);
  }
  else
  {
    mLogger.log(fmt::format("Enumerated {} VISA resources", results.size()),
                LogLevel::Info);
  }

  return results;
}

auto VISAClientResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  // TODO: Use VISA API to open; placeholder creates stub resource
  return VISAClientResource::create(mLogger, resourceString, mChannel_ptr);
}
