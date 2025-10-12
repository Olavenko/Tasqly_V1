import argparse
from datetime import datetime

def parse_args():
    """
    Parse command-line arguments for the project tree generator.
    Supports:
    - Phase & Slice selection
    - Optional comparison snapshot
    - Auto timestamping for unique snapshot IDs
    """
    parser = argparse.ArgumentParser(
        description="Generate a tagged project tree with metadata and historical phase tracking."
    )

    parser.add_argument(
        "--phase",
        type=str,
        default="Phase0",
        help="Development phase name (e.g., Phase0, Phase1, Phase2...). Default: Phase0"
    )

    parser.add_argument(
        "--slice",
        type=str,
        default=None,
        help="Optional slice name (e.g., Slice0, Slice1, Slice2...)"
    )

    parser.add_argument(
        "--compare",
        type=str,
        default=None,
        help="Optional snapshot filename (JSON) to compare against the new tree."
    )

    parser.add_argument(
        "--output",
        type=str,
        default=None,
        help="Optional custom output directory for reports (default: reports/project_tree)"
    )

    parser.add_argument(
        "--silent",
        action="store_true",
        help="Run in silent mode (no console colors or printout). Useful for CI."
    )

    args = parser.parse_args()

    # Generate a unique timestamp for current run
    args.run_ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    args.run_stamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    # Ensure slice name consistency
    if not args.slice:
        args.slice = "Slice0"

    return args
