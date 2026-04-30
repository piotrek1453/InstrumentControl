#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include "pyvisa_grpc.grpc.pb.h"
#include "pyvisa_grpc.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

class VISAClientResourceManager : public ResourceManagerIfc
{
public:
  VISAClientResourceManager(LoggerIfc &logger,
                            const std::string &serverAddress);
  ~VISAClientResourceManager() override = default;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string>;
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &mLogger;
  std::shared_ptr<grpc::Channel> mChannel_ptr;
  std::unique_ptr<pyvisa_grpc::PyVISAService::Stub> mStub;
};
