#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include "pyvisa_grpc.grpc.pb.h"
#include "pyvisa_grpc.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

class VISAClientResource : public ResourceIfc
{
public:
  explicit VISAClientResource(LoggerIfc &logger,
                              const std::string &resource_string,
                              std::shared_ptr<grpc::Channel> channel);
  ~VISAClientResource() override = default;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &mLogger;
  const std::string mResourceString;
  std::shared_ptr<grpc::Channel> mChannel_ptr;
  std::unique_ptr<pyvisa_grpc::PyVISAService::Stub> mStub;
};
