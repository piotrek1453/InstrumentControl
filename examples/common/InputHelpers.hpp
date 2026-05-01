#pragma once

#include "ifc/ResourceIfc.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#if defined(IMPLEMENTATION_ESP32)
#include "driver/uart.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

namespace example_input
{
constexpr std::size_t INPUT_BUFFER_SIZE = 512;
constexpr const char *SCPI_TERMINATOR = "\r\n";

struct CommandStep
{
  std::string command;
  void (*execute)(ResourceIfc &resource,
                  const std::string &command){nullptr};
};

inline auto executeWrite(
    ResourceIfc &resource,
    const std::string &command) -> void
{
  resource.write(command);
}

inline auto executeQuery(
    ResourceIfc &resource,
    const std::string &command) -> void
{
  resource.query(command);
}

inline auto addScpiTerminator(
    std::string command) -> std::string
{
  command += SCPI_TERMINATOR;
  return command;
}

inline auto trimTrailingNewlines(
    std::string value) -> std::string
{
  while (!value.empty() && (value.back() == '\n' || value.back() == '\r'))
  {
    value.pop_back();
  }
  return value;
}

inline auto readLine(
    const char *prompt) -> std::string
{
  if (prompt != nullptr)
  {
    std::printf("%s", prompt);
    std::fflush(stdout);
  }

#if defined(IMPLEMENTATION_ESP32)
  std::string line;
  line.reserve(INPUT_BUFFER_SIZE);

  while (true)
  {
    unsigned char ch = 0;
    int read_count =
        uart_read_bytes(static_cast<uart_port_t>(CONFIG_ESP_CONSOLE_UART_NUM),
                        &ch,
                        1,
                        portMAX_DELAY);
    if (read_count <= 0)
    {
      return {};
    }

    if (ch == '\r')
    {
      continue;
    }

    if (ch == '\n')
    {
      break;
    }

    line.push_back(static_cast<char>(ch));
    if (line.size() >= INPUT_BUFFER_SIZE - 1)
    {
      break;
    }
  }

  return line;
#else
  char buffer[INPUT_BUFFER_SIZE]{};
  if (std::fgets(buffer, sizeof(buffer), stdin) == nullptr)
  {
    return {};
  }

  return trimTrailingNewlines(std::string(buffer));
#endif
}

inline auto readCommandPlan() -> std::vector<CommandStep>
{
  std::vector<CommandStep> commands;
  std::printf(
      "Enter SCPI commands, one per line. Finish with an empty line.\n");

  while (true)
  {
    auto command = readLine("> ");
    if (command.empty())
    {
      break;
    }

    auto execute = command.contains('?') ? &executeQuery : &executeWrite;
    commands.push_back(CommandStep{
        .command = addScpiTerminator(std::move(command)), .execute = execute});
  }

  return commands;
}
} // namespace example_input
