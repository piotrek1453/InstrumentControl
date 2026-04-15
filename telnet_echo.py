#!/usr/bin/env python3
import asyncio
import logging
import telnetlib3


logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
)
logger = logging.getLogger("telnet_echo")


async def shell(reader, writer):
    peer = writer.get_extra_info("peername")
    logger.info("Client connected: %s", peer)
    while True:
        line = await reader.readline()
        if not line:
            logger.info("Client disconnected: %s", peer)
            break

        logger.info("Received from %s: %r", peer, line.rstrip("\r\n"))
        writer.write(line)
        await writer.drain()


async def main():
    logger.info("Starting telnet echo server on 0.0.0.0:6023")
    server = await telnetlib3.create_server(port=6023, shell=shell)
    await server.wait_closed()


asyncio.run(main())
