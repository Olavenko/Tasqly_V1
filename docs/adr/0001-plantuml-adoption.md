# ADR 0001: PlantUML Adoption

## Context
Tasqly requires architectural and design diagrams (Use Case, Sequence, Component) 
to support documentation and development alignment. A consistent, lightweight, 
and automatable diagramming tool is needed. Diagrams must be version-controlled, 
exportable to PNG/SVG, and integrated with CI.

## Decision
- Adopt **PlantUML** as the official diagramming tool.
- Use a corporate style configuration at `docs/uml/config/style.puml`.
- Leverage external libraries:
  - [C4-PlantUML](https://github.com/plantuml-stdlib/C4-PlantUML) for architecture diagrams.
  - [tupadr3 PlantUML Icons](https://github.com/tupadr3/plantuml-icon-font-sprites) for icons.
- Export diagrams to `docs/uml/exports/phaseX/sY/` directories.
- Provide an `uml-export.sh` script for automated exports (with timestamp check, dependency awareness).

## Consequence
- All diagrams are stored in text format, enabling diff and version control.
- Unified visual style across diagrams.
- CI workflows can auto-generate updated diagrams and publish them via GitHub Pages.
- Future-proof and consistent documentation strategy.
