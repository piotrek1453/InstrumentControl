#!/usr/bin/env python3
"""
Analyze metrics collected by launch_with_monitor.py
Generates comparison tables and plots
"""

import json
import sys
import statistics
from pathlib import Path
from typing import List, Dict
import argparse


class MetricsAnalyzer:
    """Analyze and compare metrics from multiple processes"""

    def __init__(self, metrics_dir: str = "."):
        self.metrics_dir = Path(metrics_dir)
        self.data: Dict[str, List[dict]] = {}
        self._load_metrics()

    def _load_metrics(self):
        """Load all metrics files from directory"""
        for jsonl_file in sorted(self.metrics_dir.glob("metrics_*.jsonl")):
            platform = jsonl_file.stem.replace("metrics_", "")
            metrics = []

            try:
                with open(jsonl_file) as f:
                    for line in f:
                        metrics.append(json.loads(line))
            except json.JSONDecodeError as e:
                print(f"[WARN] Invalid JSON in {jsonl_file}: {e}", file=sys.stderr)
                continue

            if metrics:
                self.data[platform] = metrics
                print(f"[OK] Loaded {len(metrics)} samples from {platform}")

    def print_summary(self):
        """Print summary statistics for each process"""
        if not self.data:
            print("[ERROR] No metrics found")
            return

        print("\n" + "=" * 80)
        print("RESOURCE METRICS SUMMARY")
        print("=" * 80)

        for platform, metrics in sorted(self.data.items()):
            self._print_platform_summary(platform, metrics)

    def _print_platform_summary(self, platform: str, metrics: List[dict]):
        """Print summary for a single platform"""
        if not metrics:
            return

        rss_values = [m["rss_mb"] for m in metrics]
        cpu_pct_values = [m.get("cpu_pct", 0) for m in metrics]
        cpu_user_ms = metrics[-1]["cpu_user_ms"]
        cpu_sys_ms = metrics[-1]["cpu_sys_ms"]

        duration_s = (metrics[-1]["ts_us"] - metrics[0]["ts_us"]) / 1e6

        print(f"\n{platform}")
        print(f"  Duration: {duration_s:.1f} seconds ({len(metrics)} samples)")
        print(f"  Memory (RSS):")
        print(f"    Min:     {min(rss_values):8.2f} MB")
        print(f"    Max:     {max(rss_values):8.2f} MB")
        print(f"    Avg:     {statistics.mean(rss_values):8.2f} MB")
        if len(rss_values) > 1:
            print(f"    StdDev:  {statistics.stdev(rss_values):8.2f} MB")

        print(f"  CPU:")
        print(f"    Avg %:   {statistics.mean(cpu_pct_values):8.2f}%")
        print(
            f"    Total:   {cpu_user_ms + cpu_sys_ms}ms (user: {cpu_user_ms}ms, sys: {cpu_sys_ms}ms)"
        )

        if metrics[-1].get("vms_mb"):
            print(f"  Virtual Memory: {metrics[-1]['vms_mb']:.2f} MB")

    def print_comparison_table(self):
        """Print comparison table of all processes"""
        if not self.data:
            return

        print("\n" + "=" * 100)
        print("COMPARISON TABLE")
        print("=" * 100)

        # Header
        print(
            f"{'Platform':<25} {'Samples':>10} {'Duration':>10} {'RSS Avg':>12} {'RSS Max':>12} {'CPU Avg':>10}"
        )
        print("-" * 100)

        for platform, metrics in sorted(self.data.items()):
            if not metrics:
                continue

            rss_values = [m["rss_mb"] for m in metrics]
            cpu_pct_values = [m.get("cpu_pct", 0) for m in metrics]
            duration_s = (metrics[-1]["ts_us"] - metrics[0]["ts_us"]) / 1e6

            print(
                f"{platform:<25} {len(metrics):>10} {duration_s:>9.1f}s {statistics.mean(rss_values):>11.2f}MB "
                f"{max(rss_values):>11.2f}MB {statistics.mean(cpu_pct_values):>9.2f}%"
            )

    def print_timeline(self, platform: str = None):
        """Print timeline of metrics for visualization"""
        if not self.data:
            return

        if platform and platform not in self.data:
            print(f"[ERROR] Platform not found: {platform}")
            return

        platforms = [platform] if platform else self.data.keys()

        print("\n" + "=" * 80)
        print("TIMELINE VIEW")
        print("=" * 80)

        for plat in platforms:
            metrics = self.data[plat]
            if not metrics:
                continue

            t0 = metrics[0]["ts_us"]

            print(f"\n{plat}:")
            print(f"{'Time (s)':<10} {'RSS (MB)':<12} {'CPU %':<10}")
            print("-" * 35)

            for m in metrics[:: max(1, len(metrics) // 20)]:  # Sample every 5% of data
                t = (m["ts_us"] - t0) / 1e6
                rss = m["rss_mb"]
                cpu = m.get("cpu_pct", 0)
                print(f"{t:<10.1f} {rss:<12.2f} {cpu:<10.2f}")

    def export_csv(self, output_file: str = "metrics.csv"):
        """Export metrics to CSV for external analysis"""
        import csv

        with open(output_file, "w", newline="") as f:
            writer = None

            for platform, metrics in sorted(self.data.items()):
                for m in metrics:
                    # Add platform to each row
                    row = {"platform": platform, **m}

                    if writer is None:
                        writer = csv.DictWriter(f, fieldnames=row.keys())
                        writer.writeheader()

                    writer.writerow(row)

        print(f"[OK] Exported to {output_file}")


def main():
    parser = argparse.ArgumentParser(
        description="Analyze metrics collected by launch_with_monitor.py",
        epilog="""
Examples:
  # Analyze all metrics in current directory
  python3 analyze_metrics.py

  # Analyze specific directory
  python3 analyze_metrics.py --dir ./results

  # Show timeline for specific platform
  python3 analyze_metrics.py --timeline VISAClient-client

  # Export to CSV
  python3 analyze_metrics.py --csv metrics.csv
        """,
    )
    parser.add_argument("--dir", default=".", help="Metrics directory")
    parser.add_argument("--timeline", help="Show timeline for specific platform")
    parser.add_argument("--csv", help="Export to CSV file")
    parser.add_argument(
        "--summary", action="store_true", default=True, help="Show summary (default)"
    )

    args = parser.parse_args()

    analyzer = MetricsAnalyzer(args.dir)

    if args.summary:
        analyzer.print_summary()
        analyzer.print_comparison_table()

    if args.timeline:
        analyzer.print_timeline(args.timeline)

    if args.csv:
        analyzer.export_csv(args.csv)


if __name__ == "__main__":
    main()
