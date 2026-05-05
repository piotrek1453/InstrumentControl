# Resource Monitoring Tools

Universal launcher and analyzer for measuring resource consumption across InstrumentControl implementations.

## Features

✅ **Non-invasive** — No modifications to application code
✅ **Pass-through stdin** — Applications get normal interactive input
✅ **Multi-process** — Launch and monitor multiple processes simultaneously
✅ **Automatic metrics** — Memory, CPU, timestamps collected to JSON Lines
✅ **Flexible configuration** — Interactive UI or JSON config files
✅ **Analysis tools** — Summary tables, timelines, CSV export

## Quick Start

### Interactive Mode (Recommended)

```bash
python3 launch_with_monitor.py
```

This will prompt you for:
1. Number of processes to launch
2. For each process:
   - Path to binary/script
   - Platform name (for labeling metrics)
   - Process type (server/client/standalone)
   - Startup delay (for clients waiting on servers)
   - Arguments to pass

Example session:
```
How many processes to launch? [1]: 2

============================================================
Process #1 Configuration
============================================================
Path to binary: python3
Platform name (e.g., VISA, VISAClient-client): VISAClient-server
Process type (server/client/standalone) [standalone]: server
Startup delay before running (seconds) [2]: 0
Arguments (space-separated, leave empty for none): lib/VISAClient/pyvisa-grpc/server/pyvisa_grpc_server.py

============================================================
Process #2 Configuration
============================================================
Path to binary: ./build/examples/VISAClient/main
Platform name (e.g., VISA, VISAClient-client): VISAClient-client
Process type (server/client/standalone) [standalone]: client
Startup delay before running (seconds) [2]: 2
Arguments (space-separated, leave empty for none):
```

### Configuration File Mode

Create `config.json`:
```json
[
  {
    "binary": "python3",
    "platform_name": "VISAClient-server",
    "process_type": "server",
    "startup_delay": 0,
    "args": ["lib/VISAClient/pyvisa-grpc/server/pyvisa_grpc_server.py"]
  },
  {
    "binary": "./build/examples/VISAClient/main",
    "platform_name": "VISAClient-client",
    "process_type": "client",
    "startup_delay": 2.0,
    "args": []
  }
]
```

Launch:
```bash
python3 launch_with_monitor.py --config config.json
```

### Analyze Results

```bash
# Summary and comparison table
python3 analyze_metrics.py

# Timeline for specific platform
python3 analyze_metrics.py --timeline VISAClient-client

# Export to CSV
python3 analyze_metrics.py --csv metrics.csv

# Analyze specific directory
python3 analyze_metrics.py --dir ./results/run1
```

## Output

### Metrics Files

For each process, a `metrics_<platform>.jsonl` file is created with one JSON object per line:

```json
{"ts_us": 1714819200000000, "plat": "VISAClient-client", "type": "client", "rss_mb": 12.34, "vms_mb": 45.67, "cpu_pct": 2.5, "cpu_user_ms": 1000, "cpu_sys_ms": 200}
```

Fields:
- `ts_us` — Unix timestamp in microseconds
- `plat` — Platform name (from configuration)
- `type` — Process type (server/client/standalone)
- `rss_mb` — Resident Set Size in MB
- `vms_mb` — Virtual Memory Size in MB
- `cpu_pct` — CPU usage percent (last 1s window)
- `cpu_user_ms` — Total user-space CPU time
- `cpu_sys_ms` — Total system CPU time

### Analysis Output

```
================================================================================
RESOURCE METRICS SUMMARY
================================================================================

VISAClient-client
  Duration: 45.2 seconds (45 samples)
  Memory (RSS):
    Min:        10.50 MB
    Max:        15.80 MB
    Avg:        12.34 MB
    StdDev:      1.25 MB
  CPU:
    Avg %:       3.50%
    Total:       1500ms (user: 1000ms, sys: 500ms)
  Virtual Memory: 45.67 MB

================================================================================
COMPARISON TABLE
================================================================================

Platform                   Samples   Duration   RSS Avg      RSS Max    CPU Avg
------------------------------------------------------------------------------------
VISAClient-client              45     45.2s   12.34 MB     15.80 MB      3.50%
VISAClient-server              45     45.2s    8.90 MB     10.20 MB      1.20%
```

## Workflow: VISAClient (C++ + Python Server)

1. **Start monitoring:**
   ```bash
   python3 launch_with_monitor.py
   ```

2. **Enter configuration:**
   - Process 1: `python3` + server script (type: server, delay: 0s)
   - Process 2: `./build/examples/VISAClient/main` (type: client, delay: 2s)

3. **Application runs normally:**
   - Python server starts first, listens on localhost:50051
   - After 2s delay, C++ client starts
   - Both processes accept stdin as usual
   - You can interact with the C++ application normally (resource strings, commands)

4. **Gather metrics:**
   - Launcher monitors both in background
   - Press Ctrl+C when done
   - Metrics saved to `metrics_VISAClient-server.jsonl` and `metrics_VISAClient-client.jsonl`

5. **Analyze:**
   ```bash
   python3 analyze_metrics.py
   ```

## Requirements

```bash
pip3 install psutil
```

## Use Cases

### 1. Measure VISA Direct Implementation
```bash
python3 launch_with_monitor.py
# Binary: ./build/examples/VISA/main
# Platform: VISA
# Type: standalone
```

### 2. Compare VISA vs VISAClient
```bash
# Run VISA
python3 launch_with_monitor.py --output ./results/visa
# ... then analyze in results/visa

# Run VISAClient
python3 launch_with_monitor.py --output ./results/visclient
# ... then analyze in results/visclient

# Compare:
python3 analyze_metrics.py --dir ./results/visa
python3 analyze_metrics.py --dir ./results/visclient
```

### 3. Measure RP2040/ESP32 (with serial logging)
```bash
# Terminal 1: Monitor device
python3 launch_with_monitor.py
# Binary: /path/to/elf
# Platform: RP2040
# Type: standalone
# (Device outputs JSON to serial, captured via stdin redirection)

# Terminal 2: Send commands via serial
picocom /dev/ttyACM0  # or appropriate device
```

### 4. Measure Multiple PC Backends
```bash
# Create config.json with VISA, VISAClient, etc.
python3 launch_with_monitor.py --config config.json --output ./comparative_run
python3 analyze_metrics.py --dir ./comparative_run
```

## Notes

- **stdin Pass-through:** All launched processes receive stdin directly from the terminal, allowing interactive input
- **stdout/stderr:** Shared with terminal for application output
- **Metrics Collection:** Runs in separate threads, minimal overhead (~1% CPU)
- **Timestamps:** All metrics timestamped in microseconds for precise correlation
- **Process Types:** "server" has no startup delay, "client" waits before starting (allows server to initialize)
