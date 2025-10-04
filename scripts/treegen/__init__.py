"""
TreeGen Package
---------------
Utility package for generating a raw project tree with metadata, tags, and snapshots.
"""

__version__ = "1.0.0"

# Expose main functions at package level for convenience
from .cli import parse_args
from .walker import walk_tree
from .console import print_status
from .exporter import export_outputs
