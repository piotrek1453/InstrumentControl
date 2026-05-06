#!/usr/bin/env python3
"""
Universal launcher with resource monitoring
- Interactive configuration
- Pass-through stdin to applications
- Simultaneous monitoring of multiple processes
- Separated metrics collection
"""

import subprocess
import psutil
import json
import time
import sys
import threading
import os
from pathlib import Path
from dataclasses import dataclass
from typing import List, Optional
import argparse


@dataclass
class ProcessConfig:
    """Configuration for a single process"""

    binary: str
    platform_name: str
    process_type: str  # "server", "client", "standalone"
    startup_delay: float = 0.0
    args: List[str] = None
    log_to_file: bool = False
    log_to_console: bool = True
    cwd: Optional[str] = None

    def __post_init__(self):
        if self.args is None:
            self.args = []


class LogReader:
    """Reads process output and writes to file and/or console"""

    def __init__(
        self,
        stream,
        log_file: Optional[str] = None,
        write_to_console: bool = True,
    ):
        self.stream = stream
        self.log_file = log_file
        self.write_to_console = write_to_console
        self.thread = None
        self.is_running = True
        self.log_handle = None

    def start(self):
        """Start reading stream in background thread"""
        if self.log_file:
            try:
                self.log_handle = open(self.log_file, "a")
            except Exception as e:
                print(
                    f"[ERROR] Failed to open log file {self.log_file}: {e}",
                    file=sys.stderr,
                )

        self.thread = threading.Thread(target=self._read_loop, daemon=True)
        self.thread.start()

    def _read_loop(self):
        """Read from stream and write to configured sinks"""
        try:
            for chunk in iter(lambda: self.stream.read(1), ""):
                if not chunk:
                    break
                if self.write_to_console:
                    # Write to console immediately, even for prompts without newline
                    sys.stdout.write(chunk)
                    sys.stdout.flush()
                # Write to file if configured
                if self.log_handle:
                    self.log_handle.write(chunk)
                    self.log_handle.flush()
        except Exception as e:
            print(f"[ERROR] Log reader error: {e}", file=sys.stderr)
        finally:
            if self.log_handle:
                self.log_handle.close()
            self.stream.close()

    def join(self, timeout: Optional[float] = None):
        """Wait for reader thread to finish"""
        if self.thread:
            self.thread.join(timeout=timeout)


class ResourceMonitor:
    """Monitors a single process"""

    def __init__(
        self,
        proc: subprocess.Popen,
        config: ProcessConfig,
        metrics_file: str,
        metrics_list: List[dict],
    ):
        self.proc = proc
        self.config = config
        self.metrics_file = metrics_file
        self.metrics_list = metrics_list
        self.is_running = True
        self.thread = None

    def start(self):
        """Start monitoring in background thread"""
        self.thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.thread.start()

    def _monitor_loop(self):
        """Main monitoring loop"""
        try:
            putil_proc = psutil.Process(self.proc.pid)
            # Initialize CPU percent with 0.5s interval for accurate measurement
            putil_proc.cpu_percent(interval=0.5)

            while self.proc.poll() is None and self.is_running:
                try:
                    ts_us = int(time.time() * 1e6)
                    mem_info = putil_proc.memory_info()
                    cpu_times = putil_proc.cpu_times()

                    metric = {
                        "ts_us": ts_us,
                        "plat": self.config.platform_name,
                        "type": self.config.process_type,
                        "rss_mb": round(mem_info.rss / 1024 / 1024, 2),
                        "vms_mb": round(mem_info.vms / 1024 / 1024, 2),
                        "cpu_pct": round(putil_proc.cpu_percent(interval=0.5), 2),
                        "cpu_user_ms": int(cpu_times.user * 1000),
                        "cpu_sys_ms": int(cpu_times.system * 1000),
                    }

                    self.metrics_list.append(metric)

                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    break

                time.sleep(0.5)

        except Exception as e:
            print(
                f"[ERROR] Monitoring {self.config.platform_name}: {e}", file=sys.stderr
            )
        finally:
            self.is_running = False

    def join(self, timeout: Optional[float] = None):
        """Wait for monitoring thread to finish"""
        if self.thread:
            self.thread.join(timeout=timeout)

    def save_metrics(self):
        """Save collected metrics to file"""
        with open(self.metrics_file, "w") as f:
            for m in self.metrics_list:
                f.write(json.dumps(m) + "\n")


class LauncherUI:
    """Interactive UI for configuration"""

    @staticmethod
    def prompt_process_config(process_num: int) -> Optional[ProcessConfig]:
        """Interactively ask for process configuration"""
        print(f"\n{'=' * 60}")
        print(f"Process #{process_num} Configuration")
        print(f"{'=' * 60}")

        binary = input("Path to binary: ").strip()
        if not binary:
            return None

        if not os.path.exists(binary):
            print(f"[ERROR] Binary not found: {binary}")
            return None

        platform_name = input("Platform name (e.g., VISA, VISAClient-client): ").strip()
        if not platform_name:
            platform_name = f"Process{process_num}"

        process_type = input(
            "Process type (server/client/standalone) [standalone]: "
        ).strip()
        if not process_type:
            process_type = "standalone"

        startup_delay = 0.0
        if process_type == "client":
            try:
                delay_str = input(
                    "Startup delay before running (seconds) [2]: "
                ).strip()
                startup_delay = float(delay_str) if delay_str else 2.0
            except ValueError:
                startup_delay = 2.0

        args_str = input("Arguments (space-separated, leave empty for none): ").strip()
        args = args_str.split() if args_str else []

        log_to_file = (
            input("Save process logs to file? (y/n) [n]: ").strip().lower() == "y"
        )

        log_to_console = (
            input("Show process logs in CLI too? (y/n) [y]: ").strip().lower() != "n"
        )

        return ProcessConfig(
            binary=binary,
            platform_name=platform_name,
            process_type=process_type,
            startup_delay=startup_delay,
            args=args,
            log_to_file=log_to_file,
            log_to_console=log_to_console,
        )

    @staticmethod
    def prompt_num_processes() -> int:
        """Ask how many processes to launch"""
        while True:
            try:
                num = int(input("\nHow many processes to launch? [1]: ").strip() or "1")
                if num > 0:
                    return num
                print("[ERROR] Must be >= 1")
            except ValueError:
                print("[ERROR] Invalid number")

    @staticmethod
    def display_summary(configs: List[ProcessConfig]):
        """Display configuration summary"""
        print(f"\n{'=' * 60}")
        print("Configuration Summary")
        print(f"{'=' * 60}")
        for i, cfg in enumerate(configs, 1):
            print(f"\n{i}. {cfg.platform_name}")
            print(f"   Binary: {cfg.binary}")
            print(f"   Type: {cfg.process_type}")
            if cfg.startup_delay:
                print(f"   Startup delay: {cfg.startup_delay}s")
            if cfg.args:
                print(f"   Args: {' '.join(cfg.args)}")
            if cfg.log_to_file:
                print(f"   Logs: YES (to file)")
                if cfg.log_to_console:
                    print(f"   Logs: YES (to CLI)")
                else:
                    print(f"   Logs: NO (CLI)")

        confirmed = input("\nContinue? (y/n) [y]: ").strip().lower()
        return confirmed != "n"


class Launcher:
    """Main launcher coordinator"""

    def __init__(self, configs: List[ProcessConfig], output_dir: str = "."):
        self.configs = configs
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.processes: List[subprocess.Popen] = []
        self.monitors: List[ResourceMonitor] = []
        self._log_readers = {}

    def launch_all(self):
        """Launch all configured processes"""
        print("\n" + "=" * 60)
        print("Launching processes...")
        print("=" * 60 + "\n")

        for i, config in enumerate(self.configs):
            if config.process_type == "client" and i > 0:
                delay = config.startup_delay
                print(
                    f"[INFO] Waiting {delay}s before starting {config.platform_name}..."
                )
                time.sleep(delay)

            self._launch_single(config, i)

        print("\n[INFO] All processes launched. Press Ctrl+C to stop.\n")

    def _launch_single(self, config: ProcessConfig, index: int):
        """Launch a single process"""
        metrics_file = self.output_dir / f"metrics_{config.platform_name}.jsonl"
        metrics_list = []
        log_file = None
        log_readers = []

        if config.log_to_file:
            log_file = self.output_dir / f"logs_{config.platform_name}.txt"

        cmd = [config.binary] + config.args
        print(f"[{index + 1}] Starting {config.platform_name}")
        print(f"    Command: {' '.join(cmd)}")
        print(f"    Metrics: {metrics_file}")
        if log_file:
            print(f"    Logs: {log_file}")

        try:
            # Pass through stdin, but capture stdout/stderr for logging
            popen_kwargs = dict(
                stdin=sys.stdin,  # Share stdin with parent
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1,  # Line buffering
            )

            if config.cwd:
                popen_kwargs["cwd"] = str(Path(config.cwd))

            proc = subprocess.Popen(cmd, **popen_kwargs)

            self.processes.append(proc)

            # Start log readers for stdout and stderr
            stdout_reader = LogReader(
                proc.stdout,
                str(log_file) if log_file else None,
                write_to_console=config.log_to_console,
            )
            stdout_reader.start()
            log_readers.append(stdout_reader)

            stderr_reader = LogReader(
                proc.stderr,
                str(log_file) if log_file else None,
                write_to_console=config.log_to_console,
            )
            stderr_reader.start()
            log_readers.append(stderr_reader)

            # Store log readers for later cleanup
            self._log_readers = getattr(self, "_log_readers", {})
            self._log_readers[proc.pid] = log_readers

            # Start monitoring
            monitor = ResourceMonitor(proc, config, str(metrics_file), metrics_list)
            monitor.start()
            self.monitors.append(monitor)

        except Exception as e:
            print(
                f"[ERROR] Failed to launch {config.platform_name}: {e}", file=sys.stderr
            )

    def wait_all(self):
        """Wait for all processes to finish"""
        try:
            for i, proc in enumerate(self.processes):
                config = self.configs[i]
                print(
                    f"[WAIT] {config.platform_name} (PID {proc.pid})", file=sys.stderr
                )
                proc.wait()
                print(
                    f"[DONE] {config.platform_name} exited with code {proc.returncode}",
                    file=sys.stderr,
                )

        except KeyboardInterrupt:
            print("\n[INFO] Terminating all processes...", file=sys.stderr)
            self._terminate_all()

    def _terminate_all(self):
        """Terminate all running processes"""
        for proc in self.processes:
            if proc.poll() is None:
                proc.terminate()

        # Give processes time to shut down gracefully
        time.sleep(1)

        for proc in self.processes:
            if proc.poll() is None:
                proc.kill()

    def finalize(self):
        """Save metrics and print summary"""
        print("\n[INFO] Finalizing...", file=sys.stderr)

        # Wait for log readers to finish
        for log_readers in self._log_readers.values():
            for reader in log_readers:
                reader.join(timeout=2)

        # Stop monitors
        for monitor in self.monitors:
            monitor.is_running = False
            monitor.join(timeout=2)

        # Save metrics
        for monitor in self.monitors:
            monitor.save_metrics()

        # Print summary
        print("\n" + "=" * 60, file=sys.stderr)
        print("Monitoring Summary", file=sys.stderr)
        print("=" * 60, file=sys.stderr)

        for i, (monitor, config) in enumerate(zip(self.monitors, self.configs), 1):
            metrics_file = self.output_dir / f"metrics_{config.platform_name}.jsonl"
            num_samples = len(monitor.metrics_list)
            print(f"\n{i}. {config.platform_name}", file=sys.stderr)
            print(f"   Metrics: {metrics_file}", file=sys.stderr)
            if config.log_to_file:
                log_file = self.output_dir / f"logs_{config.platform_name}.txt"
                print(f"   Logs: {log_file}", file=sys.stderr)
            print(f"   Samples: {num_samples}", file=sys.stderr)

            if monitor.metrics_list:
                rss_values = [m["rss_mb"] for m in monitor.metrics_list]
                print(
                    f"   Memory (RSS): {min(rss_values):.1f} - {max(rss_values):.1f} MB "
                    f"(avg: {sum(rss_values) / len(rss_values):.1f} MB)",
                    file=sys.stderr,
                )


def main():
    parser = argparse.ArgumentParser(
        description="Universal launcher with resource monitoring",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Interactive mode
  python3 launch_with_monitor.py

  # With specific output directory
  python3 launch_with_monitor.py --output ./results
        """,
    )
    parser.add_argument(
        "--output", default=".", help="Output directory for metrics files"
    )
    parser.add_argument(
        "--config", help="Load configuration from JSON file instead of prompting"
    )

    args = parser.parse_args()

    # Load configuration
    if args.config:
        with open(args.config) as f:
            configs_data = json.load(f)
        configs = [ProcessConfig(**cfg) for cfg in configs_data]
        launcher = Launcher(configs, args.output)
    else:
        # Interactive mode
        num_processes = LauncherUI.prompt_num_processes()
        configs = []

        for i in range(num_processes):
            config = LauncherUI.prompt_process_config(i + 1)
            if config:
                configs.append(config)

        if not configs:
            print("[ERROR] No valid configurations")
            return 1

        if not LauncherUI.display_summary(configs):
            print("[INFO] Cancelled")
            return 0

        launcher = Launcher(configs, args.output)

    try:
        launcher.launch_all()
        launcher.wait_all()
    finally:
        launcher.finalize()

    return 0


if __name__ == "__main__":
    sys.exit(main())
