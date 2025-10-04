import argparse

def parse_args():
    """
    Parse command-line arguments for project tree generator.
    """
    parser = argparse.ArgumentParser(
        description="Extract raw project tree with tags and metadata."
    )
    parser.add_argument(
        "--phase",
        default="Phase0",
        help="Phase name (default: Phase0)"
    )
    parser.add_argument(
        "--slice",
        default=None,
        help="Optional slice name (e.g., Slice0)"
    )
    return parser.parse_args()
