# Knowledge Base – Phase 0

This folder documents all **problems** and **decisions (ADRs)** specific to **Phase 0** of the Tasqly project.

---

## 📂 Contents
- **Problems.md**  
  A chronological log of all problems encountered during Phase 0.  
  Each entry includes:
  - Context (slice / module)
  - Symptoms
  - Root cause analysis
  - Solution + verification
  - Linked ADR for traceability

- **ADR-Summaries.md**  
  Concise summaries of all ADRs related to Phase 0.  
  Each entry includes:
  - Context + Decision + Consequences
  - Linked problems referencing this ADR

- **README.md**  
  (this file) Documentation on purpose and usage.

---

## 🎯 Purpose
- Serve as a **reference manual** for Phase 0 issues and decisions.
- Provide **traceability** between problems and ADRs.
- Help developers **solve recurring problems faster**.
- Onboard new contributors quickly with a structured overview of technical decisions.

---

## 🛠️ Usage Guidelines
1. **When a problem occurs:**  
   - Add it to `Problems.md` following the template.  
   - Include root cause, solution, and verification steps.  
   - Link it to the relevant ADR if available.

2. **When an ADR is created or updated:**  
   - Summarize it in `ADR-Summaries.md`.  
   - Link it to existing problems if relevant.  
   - Keep the summary short (context + decision + consequences).

3. **Keep consistency:**  
   - Use the same format across all phases.  
   - Problems always logged first → then linked to ADR summaries.  
   - Status field must be updated (`Solved`, `Pending`, `Planned`, `Reoccurred`).

---

## 🚀 Developer Onboarding
- Start by reading **ADR-Summaries.md** to understand key technical decisions.  
- Review **Problems.md** to see practical issues and their fixes.  
- Use this folder as the **first stop** before troubleshooting or raising a new ADR.

---

## 🔗 Next Phases
Each new phase (Phase 1, Phase 2, …) will have its own folder under:
